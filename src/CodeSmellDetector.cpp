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
#include <unordered_map>
using namespace std;

CodeSmellDetector::CodeSmellDetector(const vector<string> &linesFromFile) {
    this->linesFromFile = linesFromFile;
    this->linesFromFile.insert(this->linesFromFile.begin(), "SKIP INDEX 0");
    this->fileLineCount = linesFromFile.size();

    extractFunctions();
    detectLongMethod();
    detectLongParameterList();
    detectDuplicatedCode();

    for (const Function &function : this->functionList) {
        this->functionNames.push_back(function.getName());
    }
}

void CodeSmellDetector::extractFunctions() {
    size_t currentLineNumber = 1;
    size_t openParenLineNumber;
    size_t openCurlyLineNumber;

    while (currentLineNumber < fileLineCount) {
        skipBlankLines(currentLineNumber);
        skipLinesUntilFunctionHeader(currentLineNumber);
        openParenLineNumber = currentLineNumber;

        skipLinesUntilOpeningCurlyBracket(currentLineNumber);
        openCurlyLineNumber = currentLineNumber;

        string line = linesFromFile[currentLineNumber];
        size_t endLineNumber = findFunctionClosingCurlyBracketLine(openCurlyLineNumber);

        // Now create the function object
        vector<string> functionContent;
        extractFunctionContent(functionContent, openParenLineNumber, endLineNumber);
        Function function(functionContent);
        functionList.push_back(function);


        currentLineNumber = (openParenLineNumber == endLineNumber) ? endLineNumber + 1 : endLineNumber;
    }
}

void CodeSmellDetector::skipBlankLines(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount && linesFromFile[currentLineNumber].empty()) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilFunctionHeader(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount &&
            (!containsCharacter(linesFromFile[currentLineNumber], Function::OPENING_PAREN)
            //|| containsCharacter(linesFromFile[currentLineNumber], ';') TODO: do I need this check?
            )
        ) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilOpeningCurlyBracket(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount &&
           !containsCharacter(linesFromFile[currentLineNumber], Function::OPENING_CURLY_BRACKET)) {
        currentLineNumber++;
    }
}

bool CodeSmellDetector::containsCharacter(const string &str, const char &character) {
    return str.find(character) != string::npos;
}

size_t CodeSmellDetector::findFunctionClosingCurlyBracketLine(size_t startLineNumber) {
    // Pseudo-stack (don't need actual stack since we're not doing anything with the brackets)
    int openCurlyCount = 0;

    for (size_t currentLineNumber = startLineNumber; currentLineNumber < linesFromFile.size(); currentLineNumber++) {
        for (const char &currentChar : linesFromFile[currentLineNumber]) {
            if (currentChar == Function::OPENING_CURLY_BRACKET) {
                openCurlyCount++; // Push stack
            } else if (currentChar == Function::CLOSING_CURLY_BRACKET) {
                if (openCurlyCount == 1) {
                    return currentLineNumber; // Found matching bracket
                } else if (openCurlyCount > 0) {
                    openCurlyCount--; // Pop stack
                }
            } // else skip
        }
    }

    // Should never reach here if input file is valid syntax
    return -1;
}

void CodeSmellDetector::extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber) {
    for (size_t i = startLineNumber; i <= endLineNumber; i++) {
        string line = linesFromFile[i];
        if (line.empty() || line == "\n" || line == "\r") {
            continue;
        }

        functionContent.push_back(linesFromFile[i]);
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
    vector<pair<size_t, size_t>> alreadyCompared;

    for (size_t i = 0; i < numFunctions; i++) { // FIXME: don't need to check same function pairs
        for (size_t j = 0; j < numFunctions; j++) {
            if (isDuplicatePair(alreadyCompared, i, j)) {
                continue;
            }

            Function firstFunction = functionList[i];
            Function secondFunction = functionList[j];

            double similarityIndex = jaccardTokenSimilarityIndex(firstFunction.getFunctionBody(),
                                                                 secondFunction.getFunctionBody());
//            double similarityIndex = jaccardBiGramSimilarityIndex(firstFunction.getCodeString(),
//                                                                 secondFunction.getCodeString());
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

double CodeSmellDetector::jaccardTokenSimilarityIndex(const vector<string> &firstFunctionBody, const vector<string> &secondFunctionBody) {
    unordered_map<string, int> firstUniqueTokens;
    unordered_map<string, int> secondUniqueTokens;
    unordered_map<string, int> allUniqueTokens;
    int matchingTokens = 0;
    int totalTokens = 0;

    computeFunctionTokenCounts(firstFunctionBody, firstUniqueTokens);
    computeFunctionTokenCounts(secondFunctionBody, secondUniqueTokens);
    allUniqueTokens = getAllUniqueTokenCounts(firstUniqueTokens, secondUniqueTokens);

    // Token counts in both functions
    for (const auto &firstTokenEntry : firstUniqueTokens) {
        auto matchingTokenEntry = secondUniqueTokens.find(firstTokenEntry.first);
        // If found matching token
        if (matchingTokenEntry != secondUniqueTokens.end()) {
            matchingTokens += firstTokenEntry.second + matchingTokenEntry->second;
        }
    }

    // Total token counts in either function
    for (const auto &tokenEntry : allUniqueTokens) {
        totalTokens += tokenEntry.second;
    }

    return static_cast<double>(matchingTokens) / totalTokens;
}

void CodeSmellDetector::computeFunctionTokenCounts(const vector<string> &functionBody, unordered_map<string, int> &tokenCounts) {
    for (const string &line : functionBody) {
        istringstream iss(line);
        string token;

        while (iss >> token) {
            auto tokenEntry = tokenCounts.find(token);

            if (tokenEntry == tokenCounts.end()) {
                // Record new token
                tokenCounts.insert({token, 1});
            } else {
                // Update token count if already recorded
                tokenEntry->second++;
            }
        }
    }
}

unordered_map<string, int>
CodeSmellDetector::getAllUniqueTokenCounts(const unordered_map<string, int> &firstFunctionTokens,
                                           const unordered_map<string, int> &secondFunctionTokens) {

    unordered_map<string, int> allUniqueTokens(firstFunctionTokens);

    for (const auto& secondTokenEntry : secondFunctionTokens) {
        string token = secondTokenEntry.first;
        int count = secondTokenEntry.second;
        auto tokenEntry = allUniqueTokens.find(token);

        if (tokenEntry == firstFunctionTokens.end()) {
            // Record unique token from second function
            allUniqueTokens.insert({token, count});
        } else {
            // Matching token in both functions -> update the count
            tokenEntry->second += count;
        }
    }

    return allUniqueTokens;
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

pair<size_t, size_t> CodeSmellDetector::getSortedPair(size_t first, size_t second) {
    pair<size_t, size_t> pair(first, second);
    if (first > second) {
        swap(pair.first, pair.second);
    }
    return pair;
}

bool CodeSmellDetector::isDuplicatePair(vector<pair<size_t, size_t>> &alreadyCompared, size_t i, size_t j) {
    // Don't compare the same function
    if (i == j) {
        return true;
    }

    pair<size_t, size_t> pair = getSortedPair(i, j);

    // If we already compared these two functions
    if (find(alreadyCompared.begin(), alreadyCompared.end(), pair) != alreadyCompared.end()) {
        return true;
    }

    alreadyCompared.push_back(pair);
    return false;
}