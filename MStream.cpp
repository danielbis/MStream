//
// Created by danielbis on 11/8/19.
//

#include "MStream.h"
#include "ClusterFeatureVector.h"
#include <vector>

MStream::MStream(const double _alpha, const double _beta, unsigned int _vocabSize) {
    alpha = _alpha;
    beta = _beta;
    documentCount = 0;
    wordCount = 0;
    vocabSize = _vocabSize;
    clusters = vector<ClusterFeatureVector>(1,ClusterFeatureVector());  // one empty cluster
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

    auto it = batch.begin();
    if (documentCount == 0){
        /*
         * Create a new cluster
         * update stats
         */
        clusters.emplace_back(ClusterFeatureVector(batch[0]));
        documentCount += 1;
        wordCount += batch[0].getWordCount();
        ++it; //move on to the next sample
    }

    for (; it != batch.end(); ++it){
        /*
         * Compute the probability of document d choosing each of the
            K existing clusters and a new cluster.
            Sample cluster index z for document d according to the
            above K + 1 probabilities.
         */
        // note that each cluster probability can be computed in parallel
        // threads get clusters / thread_count clusters to compute probs


    }


}


