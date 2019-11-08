//
// Created by danielbis on 11/8/19.
//
#include <string>
#include <vector>
#include <iostream>
#include <map>
using namespace std;

#ifndef MSTREAM_DOCUMENT_H
#define MSTREAM_DOCUMENT_H


class Document {
    string documentId;
    map<int, int> wordFreq;
    unsigned int wordCount;
public:
    Document(const string&, const string&, map <string, int>&);
    string getDocId() const {return documentId;};
    const map<int, int>& getWordFreq() const {return wordFreq;};
    unsigned int getWordCount() const { return wordCount;};


};


#endif //MSTREAM_DOCUMENT_H
