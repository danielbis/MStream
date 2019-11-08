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

unsigned int vocabSize;
public:
    MStream(double, double, unsigned int);

    double newClusterProb(const Document&) const; // equation (5)
    double existingClusterProb(const Document&, unsigned int); // equation (4)
    void onePass(const vector<Document>&);
};


#endif //MSTREAM_MSTREAM_H
