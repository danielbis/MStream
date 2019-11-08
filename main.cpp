#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "Document.h"

using namespace std;
void read_data(const string& dataPath, vector<Document>& documents, map<string, int>& word2id);
int main() {

    map<string, int> word2id;
    vector<Document> documents;
    string dataPath = "/home/danielbis/Desktop/mstream/data/Tweets_clean";
    read_data(dataPath, documents, word2id);
    cout << "Number of documents created: " << documents.size() << endl;
    cout << "Vocabulary size: " << word2id.size() << endl;



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
        cout << id << "\t" << text <<endl;
        documents.emplace_back(Document(id, text, word2id));
    }
}

