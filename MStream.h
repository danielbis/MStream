//
// Created by danielbis on 11/8/19.
//
#include "Document.h"
#include "ClusterFeatureVector.h"
#include <vector>
#ifndef MSTREAM_MSTREAM_H
#define MSTREAM_MSTREAM_H


class MStream {

unsigned long int documentCount;
unsigned long int wordCount;
vector<ClusterFeatureVector> clusters;
double alpha;
double beta;
map<int, int> wordFreq;
map<string, int> doc2cluster;


unsigned int vocabSize;
public:
    MStream(double, double, unsigned int);
    void run(unsigned int iterNo, const vector<vector<Document>> &);
    void updateVocabSize();
    double newClusterProb(const Document&) const; // equation (5)
    double existingClusterProb(const Document&, unsigned int); // equation (4)
    void onePass(const vector<Document>&);

    bool getDistributions(const vector<Document> &batch, vector<vector<double>> &distributions);

    void addDocument(const vector<Document> &batch, unsigned int i, int sampledClusterIdx);
};


#endif //MSTREAM_MSTREAM_H
