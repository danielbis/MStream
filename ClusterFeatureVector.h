//
// Created by danielbis on 11/8/19.
//
#include <map>
#include "Document.h"

using namespace std;

#ifndef MSTREAM_CLUSTERFEATUREVECTOR_H
#define MSTREAM_CLUSTERFEATUREVECTOR_H


class ClusterFeatureVector {
    unsigned long int documentCount; // m_z in the paper's definition 1
    unsigned long int wordCount; // n_z in the paper's definition 1
    map<int, int> wordFreq;
    void init();

public:
    ClusterFeatureVector();
    explicit ClusterFeatureVector(const Document&);
    void addDocument(const Document&); // implements addible property
    void deleteDocument(const Document&); // implements deletable property
    unsigned long int getDocumentCount() const;
    unsigned long int getWordCount() const;
    const map<int,int>& getWordFreq() {return wordFreq;};

};


#endif //MSTREAM_CLUSTERFEATUREVECTOR_H
