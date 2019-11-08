//
// Created by danielbis on 11/8/19.
//

#include "ClusterFeatureVector.h"
#include <map>
#include <stdexcept>

using namespace std;

/*
 *  cluster feature (CF) vector implementation. Per authors, it essentially
    is a big document combined with its documents. The definition of
    the CF vector of a cluster is defined as follows.
    Definition 1:
        The cluster feature (CF) vector for a cluster z is
        defined as a tuple {nz ,mz ,nz }, where
            • nz contains a list of word frequencies in cluster z.
            • mz is the number of documents in cluster z.
            • nz is the number of words in cluster z.
            The cluster feature (CF) vector presents important addible and
            deletable properties, as described next.
            • Addible Property. A document d can be efficiently added
            to cluster z by updating its CF vector. Implemented by addDocument method
            • Deletable Property.A document d can be efficiently deleted
            from cluster z by updating its CF. Implemented by deleteDocument
*/

ClusterFeatureVector::ClusterFeatureVector() {
    documentCount = 0;
    wordCount = 0;
    wordFreq.clear();
}


ClusterFeatureVector::ClusterFeatureVector(const Document& document) {
    documentCount = 0;
    wordCount = 0;
    wordFreq.clear();
    addDocument(document);
}


void ClusterFeatureVector::addDocument(const Document & document) {

    for (auto& x: document.getWordFreq()){
        auto it = wordFreq.emplace(x.first, x.second); // if word not in cluster add to it
        // if word was already in the cluster, just increase its frequency
        if (!it.second)
            wordFreq.at(it.first->first) += x.second;

        wordCount += 1;
    }
    documentCount += 1;
}

void ClusterFeatureVector::deleteDocument(const Document& document) {
    if (documentCount <= 0)
        throw logic_error("Deleting document causes the document count to be negative.");

    for (auto& x: document.getWordFreq()){
        if (wordFreq.at(x.first) - x.second < 0)
            throw logic_error("Deleting document causes the word frequency to be negative.");
        wordFreq.at(x.first) -= x.second;
        wordCount -= x.second;
    }
    documentCount -= 1;
}

unsigned long int ClusterFeatureVector::getDocumentCount() const { return documentCount;}
unsigned long int ClusterFeatureVector::getWordCount() const { return wordCount;}

