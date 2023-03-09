//
// Created by Francis Kogge on 2/17/2023.
//

#include "CodeSmellDetector.h"
#include "Function.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <climits>
#include <algorithm>
#include <unordered_set>
#include "CodeParseUtility.h"

using namespace std;

CodeSmellDetector::CodeSmellDetector(const vector<string> &linesFromFile) {
    extractFunctions(linesFromFile);

    detectLongMethod();
    detectLongParameterList();
    detectDuplicatedCode();

    for (const Function &function : this->functionList) {
        this->functionNames.push_back(function.getName());
    }
}

void CodeSmellDetector::extractFunctions(const vector<string> &linesFromFile) {
    CodeParseUtility codeParseUtility(linesFromFile);
    vector<vector<string>> functionContentList = codeParseUtility.getFunctionContentList();

    for (const vector<string> &functionContent : functionContentList) {
        Function function(functionContent);
        functionList.push_back(function);
    }
}

void CodeSmellDetector::detectLongMethod() {
    for (const Function &function : functionList) {
        size_t functionLineCount = function.getNumberOfLinesOfCode();

        if (functionLineCount > MAX_LINES_OF_CODE) {
            LongMethod longMethod(LONG_METHOD, functionLineCount, function.getName());
            longMethodOccurrences.push_back(longMethod);
        }
    }
}

void CodeSmellDetector::detectLongParameterList() {
    for (const Function &function : functionList) {
        int parameterCount = function.getNumberOfParameters();

        if (parameterCount > MAX_PARAMETER_COUNT) {
            LongParameterList longParameterList(LONG_PARAMETER_LIST, parameterCount, function.getName());
            longParameterListOccurrences.push_back(longParameterList);
        }
    }
}

void CodeSmellDetector::detectDuplicatedCode() {
    size_t numFunctions = functionList.size();

    for (size_t i = 0; i < numFunctions - 1; i++) {
        for (size_t j = i + 1; j < numFunctions; j++) {
            Function firstFunction = functionList[i];
            Function secondFunction = functionList[j];

            double similarityIndex = jaccardBiGramSimilarityIndex(firstFunction.getCodeString(),
                                                                 secondFunction.getCodeString());
            if (similarityIndex > MAX_SIMILARITY_INDEX) {
                DuplicatedCode duplicatedCode(
                        DUPLICATED_CODE,
                        similarityIndex,
                        firstFunction.getName(),
                        secondFunction.getName()
                );
                duplicatedCodeOccurrences.push_back(duplicatedCode);
            }
        }
    }
}

double CodeSmellDetector::jaccardBiGramSimilarityIndex(const string &firstCodeString, const string &secondCodeString) {
    unordered_set<string> firstBigrams;
    unordered_set<string> secondBigrams;

    // Get bigrams for each function
    fillBigramSet(firstBigrams, firstCodeString);
    fillBigramSet(secondBigrams, secondCodeString);

    // Intersection of bigrams across both functions
    size_t matchingBigrams = 0;
    for (const string &bigram : firstBigrams) {
        // If bigram from first function is also a bigram in second function
        if (secondBigrams.find(bigram) != secondBigrams.end()) {
            matchingBigrams++;
        }
    }

    // All unique bigrams in either function
    // Initialize using first function bigrams as either is a subset
    size_t totalUniqueBigrams = firstBigrams.size();
    for (const string &bigram : secondBigrams) {
        // If we haven't recorded unique bigram from second function yet
        if (firstBigrams.find(bigram) == firstBigrams.end()) {
            totalUniqueBigrams++;
        }
    }

    return static_cast<double>(matchingBigrams) / static_cast<double>(totalUniqueBigrams);
}

void CodeSmellDetector::fillBigramSet(unordered_set<string> &bigramSet, const string &codeString) {
    for (int i = 0; i < codeString.size() - 1; i++) {
        ostringstream ss;
        ss << codeString[i] << codeString[i + 1];
        bigramSet.insert(ss.str());
    }
}

vector<string> CodeSmellDetector::getFunctionNames() const {
    return functionNames;
}

vector<CodeSmellDetector::LongParameterList> CodeSmellDetector::getLongParameterListOccurrences() const {
    return longParameterListOccurrences;
}

vector<CodeSmellDetector::DuplicatedCode> CodeSmellDetector::getDuplicateCodeOccurrences() const {
    return duplicatedCodeOccurrences;
}

vector<CodeSmellDetector::LongMethod> CodeSmellDetector::getLongMethodOccurrences() const {
    return longMethodOccurrences;
}

string CodeSmellDetector::smellTypeToString(CodeSmellDetector::SmellType type) {
    if (type == LONG_METHOD)
        return "Long Method";
    if (type == LONG_PARAMETER_LIST)
        return "Long Parameter List";
    if (type == DUPLICATED_CODE)
        return "Duplicated Code";
    else
        return "Bad type";
}


bool CodeSmellDetector::hasLongMethodSmell() const {
    return !longMethodOccurrences.empty();
}

bool CodeSmellDetector::hasLongParameterListSmell() const {
    return !longParameterListOccurrences.empty();
}

bool CodeSmellDetector::hasDuplicateCodeSmell() const {
    return !duplicatedCodeOccurrences.empty();
}