#include <random>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>      // std::rand, std::srand

#include "Document.h"
#include "MStream.h"

using namespace std;
void read_data(const string& dataPath, vector<Document>& documents, map<string, int>& word2id);
void batch_input(vector<vector<Document>>& batches, vector<Document>& data, unsigned int batchSize, bool toShuffle);

int main() {

    map<string, int> word2id;
    vector<Document> documents;
    vector<vector<Document>> batchedInput;
    unsigned int batchSize = 1500;
    string dataPath = "/Users/daniel/Desktop/FSU/advanced_db/mstream/data/Tweets-T_clean";
    read_data(dataPath, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;
    batch_input(batchedInput, documents, batchSize, false);
    double alpha = 0.025;
    double beta = 0.02;
    unsigned int iterNo = 2;
    unsigned int maxBatchesToStore = 1;
    string outputDir = "/Users/daniel/Desktop/FSU/advanced_db/mstream/results/tweets";
    MStream model(alpha, beta, outputDir, true, maxBatchesToStore);
    model.run(iterNo, batchedInput);

    return 0;
}

void read_data(const string& dataPath, vector<Document>& documents, map<string, int>& word2id){
    std::ifstream inputFile(dataPath);
    std::string line;

    while(std::getline(inputFile, line)) {     // '\n' is the default delimiter

        istringstream iss(line);
        string id;
        string text;
        getline(iss, id, '\t');   // but we can specify a different one
        getline(iss, text);
        //cout << id << "\t" << text <<endl;
        documents.emplace_back(Document(id, text, word2id));
    }
}

void batch_input(vector<vector<Document>>& batches, vector<Document>& data, unsigned int batchSize, bool toShuffle){

    unsigned int currentBatchSize = 0;
    vector<Document> singleBatch;
    singleBatch.reserve(data.size() / batchSize);
    for (auto& doc: data){
        singleBatch.emplace_back(doc);
        currentBatchSize += 1;
        if (currentBatchSize == batchSize){
            if (toShuffle)
                shuffle(singleBatch.begin(), singleBatch.end(), std::mt19937(std::random_device()()));

            batches.emplace_back(singleBatch);
            singleBatch.clear();
            currentBatchSize = 0;
        }
    }
    if (currentBatchSize > 0){
        if (toShuffle)
            shuffle(singleBatch.begin(), singleBatch.end(), std::mt19937(std::random_device()()));

        batches.emplace_back(singleBatch);
    }
}

