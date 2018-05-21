/*
 * Author: Nabi Ozberkman
 * Filename: LanguageModel.cpp
 */
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

bool myCompare(pair<string, float>& a, pair<string, float>& b) {
    return a.second > b.second;
}

int main(int argc, char* agrv[]) {
    ifstream wordFile, countFile;
    string line;
    int x;
    vector<pair<string, int> > countArr;

    wordFile.open("vocab.txt");
    countFile.open("unigram.txt");

    //Getting unigram counts
    while (true) {
        wordFile >> line;
        countFile >> x;

        if(wordFile.eof())
            break;
        countArr.push_back(make_pair(line, x));
    }

    wordFile.close();
    countFile.close();

    //Adding unigram counts
    float total = 0.0;
    for (auto it = countArr.begin(); it != countArr.end(); it++) {
        total += (*it).second;
    }

    vector<pair<string, float> > probArr;

    //Calculating Unigram probabilities
    for (auto it = countArr.begin(); it != countArr.end(); it++) {
        probArr.push_back(make_pair((*it).first, ((float)((*it).second))/total));
    }



    vector<pair<string, int> > biCount(probArr.size());
    vector<pair<string, vector<pair<string, float> > > > biProb(probArr.size());

    ifstream bigramFile;
    bigramFile.open("bigram.txt");

    int index1;
    int index2;
    int indexCurr = 1;
    int count;
    int totalCount;
    bool first = true;

    //Getting bigram counts and totalling them for each w1;
    while (true) {
        if (!bigramFile.eof()) {
            bigramFile >> index1;
            bigramFile >> index2;
            bigramFile >> count;
        }
        if (first) {
            indexCurr = index1;
            first = false;
        }

        if (index1 != indexCurr || bigramFile.eof()) {
            biCount[indexCurr-1] = (make_pair(probArr[indexCurr-1].first, totalCount));
            indexCurr = index1;
            totalCount = 0;
            if(bigramFile.eof())
                break;
        }
        totalCount += count;
    }



    //cout << biCount[0].first << "-------" << biCount[0].second << endl;

    bigramFile.close();

    bigramFile.open("bigram.txt");

    indexCurr = 1;
    first = true;

    //Calculating bigram  probabilities
    vector<pair<string, float> > k;
    while (true) {
        if (!bigramFile.eof()) {
            bigramFile >> index1;
            bigramFile >> index2;
            bigramFile >> count;

            if(first) {
                indexCurr = index1;
                first = false;
            }
        }
        else
            break;

        if (index1 != indexCurr) {
            biProb[indexCurr-1] = (make_pair(probArr[indexCurr-1].first, k));
            k.clear();
            indexCurr = index1;
            //cout << "HEREE!!!!!!!!!!!!!!!"  << k.size() << endl;
        }
        k.push_back(make_pair(probArr[index2-1].first, ((float)count)/((float)(biCount[index1-1].second))));
    }


    //Sorting bigram probabilities
    for (auto it = biProb.begin(); it != biProb.end(); it++) {
        sort((*it).second.begin(), (*it).second.end(), myCompare);
    }

    //Sorting unigram probabilities
    sort(probArr.begin(), probArr.end(), myCompare);


    //Printing unigram probabilities for words starting with 'S'
    for (auto it = probArr.begin(); it != probArr.end(); it++) {
        if ((*it).first.at(0) == 'S')
            cout << (*it).first << "-----" << (*it).second << endl;
    }

    cout << "++++++++++++++++++++++++++++++++++" << endl;

    int l = 3;

    //Printing bigram probabilities for top 10 words that follow "THE"
    for (int i = 0; i < 10; i++) {
        cout << biProb[l].first << "-----__" <<biProb[l].second[i].first << "-------" << biProb[l].second[i].second << endl;
    }
    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    unordered_map<string, float> uProb;

    //Transferring unigram probabilities to a map for easy search
    uProb.insert(probArr.begin(), probArr.end());

    //Calculating likelihood of sentence 1
    float sentenceU = (uProb.find("THE"))->second;
    sentenceU *= (uProb.find("STOCK"))->second;
    sentenceU *= (uProb.find("MARKET"))->second;
    sentenceU *= (uProb.find("FELL"))->second;
    sentenceU *= (uProb.find("BY"))->second;
    sentenceU *= (uProb.find("ONE"))->second;
    sentenceU *= (uProb.find("HUNDRED"))->second;
    sentenceU *= (uProb.find("POINTS"))->second;
    sentenceU *= (uProb.find("LAST"))->second;
    sentenceU *= (uProb.find("WEEK"))->second;



    cout << "Unigram likelihood of sentece 1: " << log(sentenceU) << endl;

    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    //Transferring bigram probabilities to a map for easy search
    unordered_map<string, unordered_map<string, float> > bProb;

    for (auto it = biProb.begin(); it != biProb.end(); it++) {
        unordered_map<string, float> s;
        s.insert((*it).second.begin(), (*it).second.end());
        bProb.insert(make_pair((*it).first, s));
        s.clear();
    }

    //Realized that using a vector for the sentence is easier
    vector<string> sentenceArr = {"<s>","THE", "STOCK", "MARKET", "FELL", "BY", "ONE", "HUNDRED", "POINTS", "LAST", "WEEK"};
    float sentenceB = 1.0;
    //Calculating bigram likelihood of sentence 1
    for (int i = 0; i < sentenceArr.size()-1; i++) {
        if(bProb.find(sentenceArr[i]) != bProb.end() && bProb.find(sentenceArr[i])->second.find(sentenceArr[i+1]) != bProb.find(sentenceArr[i])->second.end())
            sentenceB *= (bProb.find(sentenceArr[i]))->second.find(sentenceArr[i+1])->second;
    }

    cout << "Bigram likelihood of sentence 1: " << log(sentenceB) << endl;

    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    //Saving sentence 2 into a vector
    vector<string> sentenceArr2 = {"<s>", "THE", "SIXTEEN", "OFFICIALS", "SOLD", "FIRE", "INSURANCE"};

    float sentenceU2 = 1.0;

    //Calculating unigram likelihood of sentence 2
    for (int i = 1; i < sentenceArr2.size(); i++) {
        sentenceU2 *= uProb.find(sentenceArr2[i])->second;
    }

    cout << "Unigram likelihood of sentence 2: " << log(sentenceU2) << endl;

    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    float sentenceB2 = 1.0;

    //Calculating bigram likelihood of sentence 2
    for (int i = 0; i < sentenceArr2.size()-1; i++) {
        if(bProb.find(sentenceArr2[i]) != bProb.end() && bProb.find(sentenceArr2[i])->second.find(sentenceArr2[i+1]) != bProb.find(sentenceArr2[i])->second.end())
            sentenceB2 *= (bProb.find(sentenceArr2[i]))->second.find(sentenceArr2[i+1])->second;
        else {
            sentenceB2 *= 0;
            //Printing pairs that are not observed in the training set
            cout << "Word pair unobserved: " << sentenceArr2[i] << " + " << sentenceArr2[i+1] << endl;
        }
    }

    cout << "Bigram likelihood of sentence 2: " << log(sentenceB2) << endl;

    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    vector<float> uP;
    vector<float> bP;
    vector<float> rP;

    //Calculating unigram and bigram probabilities for each word and pair for sentence 2
    //in the 2 following for loops.
    for (int i = 1; i < sentenceArr2.size(); i++) {
        uP.push_back(uProb.find(sentenceArr2[i])->second);
    }

    for (int i = 0; i < sentenceArr2.size()-1; i++) {
        if(bProb.find(sentenceArr2[i]) != bProb.end() && bProb.find(sentenceArr2[i])->second.find(sentenceArr2[i+1]) != bProb.find(sentenceArr2[i])->second.end())
            bP.push_back((bProb.find(sentenceArr2[i]))->second.find(sentenceArr2[i+1])->second);
        else {
            bP.push_back(0);
        }
    }

    ofstream resultFile;
    resultFile.open("results.txt");

    //Printing results of joint likelihood for sentence 2 with varying lambda to a text file
    for (float m = 0.0; m <= 1.0; m += 0.001) {
        for (int k = 0; k < uP.size(); k++) {
            rP.push_back(((1-m)*uP[k]) + (m*bP[k]));
        }
        float logLikelihood = 1.0;
        for (auto it = rP.begin(); it != rP.end(); it++) {
            logLikelihood *= *it;
        }
        logLikelihood = log(logLikelihood);
        rP.clear();
        resultFile << m << "\t" << logLikelihood << endl;

    }


}
