//
// Created by danielbis on 11/8/19.
//

#include "MStream.h"
#include "ClusterFeatureVector.h"
#include <vector>
#include <random>
#include <fstream>
#include <chrono>
#include <algorithm>

MStream::MStream(const double _alpha, const double _beta, const string &_outputDir, const bool _mStreamF=false,
                 const unsigned int _batchesToStore=0) {
    alpha = _alpha;
    beta = _beta;
    documentCount = 0;
    wordCount = 0;
    vocabSize = 0;
    //clusters  // one empty cluster
    outputDir = _outputDir;
    storedBatches = 0;
    batchesToStore = _batchesToStore;
    mStreamF = _mStreamF;

}

void MStream::run(const unsigned int iterNo, const vector<vector<Document>> &batches) {

    // run first iter
    int currentBatchNo = 0;
    auto start = chrono::high_resolution_clock::now();
    vector<chrono::seconds > runTimes;
    for (auto& batch: batches){
        if (mStreamF){
            if (storedBatches > batchesToStore)
                deleteBatch(batches[currentBatchNo-batchesToStore-1]);
        }
        unsigned int iteration = 1;
        for (; iteration <= iterNo; ++iteration) {
            if (iteration == 1){
                onePass(batch, false, false);
                if (iterNo == 1)
                    output(batch, currentBatchNo);
            }
            else if (iteration == iterNo) {
                onePass(batch, true, true);
                // save output after every batch
                output(batch, currentBatchNo);
            }
            else {
                onePass(batch, true, false);
            }
        }
        currentBatchNo += 1;
        storedBatches += 1;
        auto batch_time = chrono::high_resolution_clock::now();
        auto current_dur = chrono::duration_cast<chrono::seconds>(batch_time - start);
        runTimes.push_back(current_dur);

    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);

    summary_output(duration, runTimes);
}

double MStream::newClusterProb(const Document & document) const{
    /*
     * Implements equation (5) of the paper
     */
    double pseudoDocs = alpha * documentCount;
    double leftSide = pseudoDocs / (documentCount-1 + pseudoDocs);

    double topRight = 1;
    double bottomRight = 1;
    for (auto& x: document.getWordFreq()){
        for (unsigned int j = 1; j <= x.second; ++j){
            topRight = topRight * (beta + j - 1);
        }
    }

    for (unsigned int i=1; i <= document.getWordCount(); ++i){
        bottomRight = bottomRight * (vocabSize*beta + i - 1);
    }

    // equation (5) returned
    return leftSide* (topRight / bottomRight);
}

double MStream::existingClusterProb(const Document & document, unsigned int cluster_idx) {
    /*
     * Implements equation (4) of the paper
     */

    unsigned long int noWords = clusters[cluster_idx].getWordCount(); // n_z in the paper, words in cluster
    double topRight = 1;
    double bottomRight = 1;
    double leftSide = clusters[cluster_idx].getDocumentCount() / (documentCount-1 + (alpha*documentCount));

    int freqOfWinZ = 0;
    for (auto& x: document.getWordFreq()){ //product over words in d
        auto clusterFreqMapIt = clusters[cluster_idx].getWordFreq().find(x.first);
        if (clusterFreqMapIt == clusters[cluster_idx].getWordFreq().end())
            freqOfWinZ = 0;
        else
            freqOfWinZ = clusterFreqMapIt->second;

        for (unsigned int j = 1; j <= x.second; ++j){ //product over occurrences of a word in d
            topRight = topRight * (freqOfWinZ + beta + j - 1);
        }
    }

    for (unsigned int i=1; i <= document.getWordCount(); ++i){
        bottomRight = bottomRight * (noWords + vocabSize*beta + i - 1);
    }

    return leftSide * (topRight/bottomRight);
}


void MStream::onePass(const vector<Document> &batch, const bool delete_first, const bool lastIter) {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    for (auto& batchIter: batch){
        if (delete_first) // this is true for iteration number > 1
            deleteDocument(batchIter);

        vector <double> distribution;
        bool wasEmpty = getDistributions(batchIter, distribution);
        if (wasEmpty){
            addDocument(batchIter, 0);
        }
        else {
            if (lastIter)  // greedy
            {
                int sampledClusterIdx = distance(distribution.begin(), max_element(distribution.begin(), distribution.end()));
                addDocument(batchIter, sampledClusterIdx);
            }
            else {
                discrete_distribution<int> docDistribution(distribution.begin(), distribution.end());
                int sampledClusterIdx = docDistribution(generator);
                addDocument(batchIter, sampledClusterIdx);
            }
        }

    }


}

void MStream::addDocument(const Document &document, int sampledClusterIdx) {
    if (sampledClusterIdx >= clusters.size())  // new cluster selected
        clusters.emplace_back(ClusterFeatureVector(document));
    else  // existing cluster selected
        clusters[sampledClusterIdx].addDocument(document);

    // update documentId to cluster mapping
    auto it = doc2cluster.emplace(document.getDocId(), sampledClusterIdx);
    if (!it.second)
        doc2cluster.at(it.first->first) = sampledClusterIdx;

    // Update the vocabulary
    for (auto& x: document.getWordFreq()){
        auto vocabIt = wordFreq.emplace(x.first, x.second); // if word not in cluster add to vocabIt
        // if word was already in the cluster, just increase its frequency
        if (!vocabIt.second) {
            if (vocabIt.first->second == 0){
                vocabSize += 1;
            }
            wordFreq.at(vocabIt.first->first) += x.second;
        }
        else  // new word --> increase vocabSize
            vocabSize += 1;

        wordCount += x.second;
    }
    // update the documents count
    documentCount += 1;

}

void MStream::deleteDocument(const Document &document) {
    int old_wordcount = wordCount;
    int oldClusterIdx = doc2cluster.at(document.getDocId());
    unsigned long clusterOldCount = clusters[oldClusterIdx].getWordCount();
    clusters[oldClusterIdx].deleteDocument(document); // remove from cluster
    if (clusters[oldClusterIdx].getWordCount() == clusterOldCount)
        throw logic_error("No words deleted from cluster");
    // remove from MStream global stats
    for (auto& x: document.getWordFreq()){
        if (wordFreq.at(x.first) - x.second < 0)
            throw logic_error("Deleting document causes the word frequency to be negative.");
        wordFreq.at(x.first) -= x.second;
        wordCount -= x.second;
        // update vocab size
        if (wordFreq.at(x.first) == 0){
            vocabSize -= 1;
        }
    }
    if (wordCount == old_wordcount){
        throw logic_error("No words deleted from MStream");
    }
    documentCount -= 1;

}

void MStream::deleteBatch(const vector<Document>& batch){
    for (auto& document: batch){
        deleteDocument(document);
    }
    storedBatches -= 1;

}


bool MStream::getDistributions(const Document &document, vector<double> &distribution) {

    bool wasEmpty = false;
    if (documentCount == 0){
        /*
         * Create a new cluster
         * update stats
         */

        return true; //move on to the next sample
    }

    /*
     * Compute the probability of document d choosing each of the
        K existing clusters and a new cluster.
        Sample cluster index z for document d according to the
        above K + 1 probabilities.
     */

    for (unsigned int i=0; i < clusters.size(); ++i){
        distribution.push_back(existingClusterProb(document, i));
    }
    distribution.push_back(newClusterProb(document));


    return wasEmpty;
}

void MStream::output(const vector<Document> &batch, unsigned int batchNo) const {

    string out_path = outputDir + "/" + "batch"+ to_string(batchNo)+ ".txt";
    ofstream myfile;
    myfile.open (out_path);
    for(auto& doc: batch){
        int clusteridx = doc2cluster.at(doc.getDocId());
        myfile << doc.getDocId() << "\t" << clusteridx << "\n";
    }
    myfile.close();
}

void MStream::summary_output(chrono::seconds dur, const vector<chrono::seconds> &runTimes) const{
    string out_path = outputDir + "/summary.txt";
    ofstream myfile;
    myfile.open (out_path);
    myfile << "Duration:" << "\t" << dur.count() << "\n";
    myfile << "ClusterCount:" <<"\t" << countClusters() << "\n";
    myfile << "DurationsPerBatch:" << "\t";
    for (auto& x: runTimes){
        myfile << x.count() << "\t";
    }
}

int MStream::countClusters() const {
    int counter = 0;

    for (auto& x: clusters){
        if(x.getDocumentCount() > 0){
            counter += 1;
        }
    }
    return counter;
}

