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
string outputDir;

unsigned int vocabSize;
public:
    MStream(const double, const double, const string &_outputDir);
    void run(const unsigned int iterNo, const vector<vector<Document>> &);

    double newClusterProb(const Document&) const; // equation (5)
    double existingClusterProb(const Document&, unsigned int); // equation (4)
    void onePass(const vector<Document> &, const bool delete_first, const bool lastIter);

    bool getDistributions(const Document &document, vector<double> &distribution);

    void addDocument(const Document &document, int sampledClusterIdx);
    void deleteDocument(const Document &document);
    void output(const vector<Document> &batch, unsigned int batchNo);
};


#endif //MSTREAM_MSTREAM_H
