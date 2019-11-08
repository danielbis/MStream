//
// Created by danielbis on 11/8/19.
//

#include <sstream>
#include <iterator>
#include "Document.h"
using namespace std;

Document::Document(const string & _documentId, const string & _text, map<string, int>& word2Id) {

    size_t vocabSize = word2Id.size();
    wordCount = 0;
    documentId = _documentId;
    // tokenize input
    istringstream iss(_text);
    vector<string> tokens(istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    // convert input to ids

    for (auto& x: tokens) {
        // if word not in dictionary add it and increment vocab size
        auto it = word2Id.emplace(x, vocabSize);
        if (it.second)
            vocabSize += 1;
        // try to add word to frequency map
        auto itF = wordFreq.emplace(it.first->second, 1);
        // if it is already there, increase the frequency
        if (!itF.second) {
            wordFreq.at(it.first->second) += 1;
        }

        wordCount += 1;
    }
}