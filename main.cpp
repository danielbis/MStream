#include <random>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>      // std::rand, std::srand
#include <algorithm>
#include <iomanip>

#include "Document.h"
#include "MStream.h"

using namespace std;
const int TWEET_SIZE = 30322;
const int NEWS_SIZE = 11109;
const string DATA_PATH = "/home/danielbis/Desktop/mstream/data/";
const string RESULTS_PATH = "/home/danielbis/Desktop/mstream/results/";
void read_data(const string& dataPath, vector<Document>& documents, map<string, int>& word2id);
void batch_input(vector<vector<Document>>& batches, vector<Document>& data, unsigned int batchSize, bool toShuffle);
void test_iterations(const string& datasetName, int noBatches, double alpha, double beta,\
                        int toStore);
void testBatchSize(const string& datasetName, const int noIter, const double alpha, const double beta,\
                        const int toStore);
void testAlphas(const string& datasetName, const int noIter, const int noBatches, const double beta,\
                        const int toStore);
void testBetas(const string& datasetName, const int noIter, const int noBatches, const double alpha,\
                        const int toStore);
void testStoredBatches(const string& datasetName, const int noBatches, const double alpha, const double beta,\
                        const int iterNo);

int main() {


    double alpha = 0.03;
    double beta = 0.03;
    int iterNo = 10;
    int batchesNo = 16;
    int toStore = 1;

    vector<string> datasetNames = { "tweets", "news","tweets_t", "news_t"};
    for (auto& datasetName: datasetNames){
        test_iterations(datasetName, batchesNo, alpha, beta, toStore);
        testBatchSize(datasetName, iterNo, alpha, beta, toStore);
        testAlphas(datasetName, 2, 16, beta, toStore);
        testBetas(datasetName, 2, 16, alpha, toStore);
        testStoredBatches(datasetName, 16, alpha,beta,iterNo);
    }


    return 0;
}

void test_iterations(const string& datasetName, const int noBatches, const double alpha, const double beta,\
                        const int toStore){

    unsigned int batchSize = 0;
    bool _shuffle = false;
    bool useMstreamF = false;
    string outputDir = RESULTS_PATH + datasetName +"/iterations/";
    if (datasetName == "tweets" || datasetName == "tweets_t")
        batchSize = ceil(TWEET_SIZE / noBatches);
    else if (datasetName == "news" || datasetName == "news_t")
        batchSize = ceil(NEWS_SIZE / noBatches);
    else {
        cout << "Dataset: " << datasetName << " not supported, exiting...\n";
        exit(0);
    }
    if (datasetName == "news_t" || datasetName == "tweets_t"){
        _shuffle = true;
        useMstreamF = true;
    }
    map<string, int> word2id;
    vector<Document> documents;
    vector<vector<Document>> batchedInput;
    read_data(DATA_PATH + datasetName, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;
    batch_input(batchedInput, documents, batchSize, _shuffle);

    for (int i=1; i<=10; i++){
        cout << "Running with " << i << " iterations.\n";
        string temp_out = outputDir + "iter_" + to_string(i);
        cout << "Saving to " << temp_out << "\n";
        MStream model(alpha, beta, temp_out, useMstreamF, toStore);
        model.run(i, batchedInput);
    }


}

void testBatchSize(const string& datasetName, const int noIter, const double alpha, const double beta,\
                        const int toStore){
    int datasetSize = 0;
    unsigned int batchSize = 0;
    bool _shuffle = false;
    bool useMstreamF = false;
    string outputDir =RESULTS_PATH + datasetName + "/batch_no/";
    if (datasetName == "tweets" || datasetName == "tweets_t")
        datasetSize = TWEET_SIZE;
    else if (datasetName == "news" || datasetName == "news_t")
        datasetSize = NEWS_SIZE;
    else {
        cout << "Dataset: " << datasetName << " not supported, exiting...\n";
        exit(0);
    }
    if (datasetName == "news_t" || datasetName == "tweets_t"){
        _shuffle = true;
        useMstreamF = true;
    }
    map<string, int> word2id;
    vector<Document> documents;
    vector<vector<Document>> batchedInput;
    read_data(DATA_PATH + datasetName, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;

    for (int i=5; i<=30; i++){
        cout << "Running with " << i << " batches.\n";
        batchSize = ceil(datasetSize / i);
        batch_input(batchedInput, documents, batchSize, _shuffle);
        string temp_out = outputDir + "batches_" + to_string(i);
        cout << temp_out << endl;

        MStream model(alpha, beta, temp_out, useMstreamF, toStore);
        model.run(noIter, batchedInput);
    }
}

void testAlphas(const string& datasetName, const int noIter, const int noBatches, const double beta,\
                        const int toStore){
    unsigned int batchSize = 0;
    bool _shuffle = false;
    bool useMstreamF = false;
    string outputDir = RESULTS_PATH + datasetName +"/alpha/";
    if (datasetName == "tweets" || datasetName == "tweets_t")
        batchSize = ceil(TWEET_SIZE / noBatches);
    else if (datasetName == "news" || datasetName == "news_t")
        batchSize = ceil(NEWS_SIZE / noBatches);
    else {
        cout << "Dataset: " << datasetName << " not supported, exiting...\n";
        exit(0);
    }
    if (datasetName == "news_t" || datasetName == "tweets_t"){
        _shuffle = true;
        useMstreamF = true;
    }
    map<string, int> word2id;
    vector<Document> documents;
    vector<vector<Document>> batchedInput;
    read_data(DATA_PATH + datasetName, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;
    batch_input(batchedInput, documents, batchSize, _shuffle);
    double alpha = 0.01;
    int counter = 10;
    while (alpha < 0.051){
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << alpha;
        std::string s_alpha = stream.str();
        cout << "Running with alpha: " << s_alpha << ".\n";
        string temp_out = outputDir + "a_" + to_string(counter);
        cout << "Saving to " << temp_out << "\n";
        MStream model(alpha, beta, temp_out, useMstreamF, toStore);
        model.run(noIter, batchedInput);
        alpha += 0.001;
        counter += 1;
    }
}

void testBetas(const string& datasetName, const int noIter, const int noBatches, const double alpha,\
                        const int toStore){
    unsigned int batchSize = 0;
    bool _shuffle = false;
    bool useMstreamF = false;
    string outputDir = RESULTS_PATH + datasetName +"/beta/";
    if (datasetName == "tweets" || datasetName == "tweets_t")
        batchSize = ceil(TWEET_SIZE / noBatches);
    else if (datasetName == "news" || datasetName == "news_t")
        batchSize = ceil(NEWS_SIZE / noBatches);
    else {
        cout << "Dataset: " << datasetName << " not supported, exiting...\n";
        exit(0);
    }
    if (datasetName == "news_t" || datasetName == "tweets_t"){
        _shuffle = true;
        useMstreamF = true;
    }
    map<string, int> word2id;
    vector<Document> documents;
    vector<vector<Document>> batchedInput;
    read_data(DATA_PATH + datasetName, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;
    batch_input(batchedInput, documents, batchSize, _shuffle);
    double beta = 0.01;
    int counter = 10;
    while (beta < 0.051){
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << beta;
        std::string s_beta = stream.str();
        cout << "Running with beta: " << s_beta << ".\n";
        string temp_out = outputDir + "b_" + to_string(counter);
        cout << "Saving to " << temp_out << "\n";
        MStream model(alpha, beta, temp_out, useMstreamF, toStore);
        model.run(noIter, batchedInput);
        beta += 0.001;
        counter += 1;
    }
}

void testStoredBatches(const string& datasetName, const int noBatches, const double alpha, const double beta,\
                        const int iterNo){

    unsigned int batchSize = 0;
    bool _shuffle = false;
    bool useMstreamF = false;
    string outputDir = RESULTS_PATH + datasetName +"/stored_no/";
    if (datasetName == "tweets_t")
        batchSize = ceil(TWEET_SIZE / noBatches);
    else if (datasetName == "news_t")
        batchSize = ceil(NEWS_SIZE / noBatches);
    else {
        cout << "Dataset: " << datasetName << " not supported, exiting...\n";
        exit(0);
    }

    _shuffle = true;
    useMstreamF = true;

    map<string, int> word2id;
    vector<Document> documents;
    vector<vector<Document>> batchedInput;
    read_data(DATA_PATH + datasetName, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;
    batch_input(batchedInput, documents, batchSize, _shuffle);

    for (int i=0; i<16; i++){
        cout << "Running with " << i << " batches stored.\n";
        string temp_out = outputDir + "s_" + to_string(i);
        cout << "Saving to " << temp_out << "\n";
        MStream model(alpha, beta, temp_out, useMstreamF, i);
        model.run(iterNo, batchedInput);
    }
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

