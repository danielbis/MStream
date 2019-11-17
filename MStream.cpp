//
// Created by danielbis on 11/8/19.
//

#include "MStream.h"
#include "ClusterFeatureVector.h"
#include <vector>
#include <random>

MStream::MStream(const double _alpha, const double _beta, unsigned int _vocabSize) {
    alpha = _alpha;
    beta = _beta;
    documentCount = 0;
    wordCount = 0;
    vocabSize = _vocabSize;
    clusters = vector<ClusterFeatureVector>(1,ClusterFeatureVector());  // one empty cluster
}

void MStream::run(unsigned int iterNo, const vector<vector<Document>> & batches) {

    // run first iter
    for (auto& batch: batches){
        onePass(batch);
    }
    unsigned int iteration = 2;
    for (; iteration <= iterNo; ++iteration){

    }



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
    double leftSide = clusters[cluster_idx].getDocumentCount() / documentCount-1 + alpha*documentCount;

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


void MStream::onePass(const vector<Document> & batch) {

    vector <vector <double>> distributions;
    bool wasEmpty = getDistributions(batch, distributions);


    auto distrIter = distributions.begin();
    unsigned int i = 0;
    if (wasEmpty)
        i += 1;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    for (; distrIter != distributions.end(); ++distrIter){
        discrete_distribution<int>docDistribution(distrIter->begin(), distrIter->end()) ;
        int sampledClusterIdx = docDistribution(generator);
        addDocument(batch, i, sampledClusterIdx);

    }


}

void MStream::addDocument(const vector<Document> &batch, unsigned int i, int sampledClusterIdx) {
    if (sampledClusterIdx >= clusters.size())  // new cluster selected
        clusters.emplace_back(ClusterFeatureVector(batch[i]));
    else  // existing cluster selected
        clusters[sampledClusterIdx].addDocument(batch[i]);

    // update documentId to cluster mapping
    auto it = doc2cluster.emplace(batch[i].getDocId(), sampledClusterIdx);
    if (!it.second)
        doc2cluster.at(it.first->first) = sampledClusterIdx;

    // Update the vocabulary
    for (auto& x: batch[i].getWordFreq()){
        auto it = wordFreq.emplace(x.first, x.second); // if word not in cluster add to it
        // if word was already in the cluster, just increase its frequency
        if (!it.second)
            wordFreq.at(it.first->first) += x.second;

        wordCount += x.second;
    }
    // update the documents count
    documentCount += 1;

}

bool MStream::getDistributions(const vector<Document> &batch, vector<vector<double>> &distributions) {
    auto documentsIter = batch.begin();
    bool wasEmpty = false;
    if (documentCount == 0){
        /*
         * Create a new cluster
         * update stats
         */
        wasEmpty = true;
        clusters.emplace_back(ClusterFeatureVector(batch[0]));
        documentCount += 1;
        wordCount += batch[0].getWordCount();
        ++documentsIter; //move on to the next sample
    }

    for (; documentsIter != batch.end(); ++documentsIter){
        vector <double> single_distribution;
        /*
         * Compute the probability of document d choosing each of the
            K existing clusters and a new cluster.
            Sample cluster index z for document d according to the
            above K + 1 probabilities.
         */
        // note that each cluster probability can be computed in parallel
        // threads get clusters / thread_count clusters to compute probs
        for (unsigned int i=0; i < clusters.size(); ++i){
            single_distribution.push_back(existingClusterProb(*documentsIter, i));
        }
        single_distribution.push_back(newClusterProb(*documentsIter));
        distributions.emplace_back(single_distribution);
    }

    return wasEmpty;
}

void MStream::updateVocabSize() {

    unsigned int temp = 0;
    for (auto& it: wordFreq){
        if (it.second > 0){
            temp += 1;
        }
    }
    vocabSize = temp;
}


