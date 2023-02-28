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

const char CodeSmellDetector::OPENING_CURLY_BRACKET = '{';
const char CodeSmellDetector::CLOSING_CURLY_BRACKET = '}';
const char CodeSmellDetector::OPENING_PAREN = '(';


CodeSmellDetector::CodeSmellDetector(const vector<string> &linesFromFile) {
    this->linesFromFile = linesFromFile;
    this->linesFromFile.insert(this->linesFromFile.begin(), "SKIP INDEX 0");
    this->lineCount = linesFromFile.size();

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

    while (currentLineNumber < lineCount) {
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
    while (currentLineNumber < lineCount && linesFromFile[currentLineNumber].empty()) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilFunctionHeader(size_t &currentLineNumber) {
    while (currentLineNumber < lineCount &&
            (!containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_PAREN)
            //|| containsCharacter(linesFromFile[currentLineNumber], ';') TODO: do I need this check?
            )
        ) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilOpeningCurlyBracket(size_t &currentLineNumber) {
    while (currentLineNumber < lineCount &&
           !containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_CURLY_BRACKET)) {
        currentLineNumber++;
    }
}

bool CodeSmellDetector::containsCharacter(const string &str, const char &character) {
    return str.find(character) != string::npos;
}

size_t CodeSmellDetector::findFunctionClosingCurlyBracketLine(size_t startLineNumber) {
    // Pseudo-stack (don't actually need stack since
    // we're not doing anything with the brackets)
    size_t openCurlyCount = 0;

    for (size_t currentLineNumber = startLineNumber; currentLineNumber < linesFromFile.size(); currentLineNumber++) {
        for (const char &currentChar : linesFromFile[currentLineNumber]) {
            if (currentChar == CodeSmellDetector::OPENING_CURLY_BRACKET) {
                openCurlyCount++; // Push stack
            } else if (currentChar == CodeSmellDetector::CLOSING_CURLY_BRACKET) {
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
        size_t lineCount = function.getNumberOfLinesOfCode();
        if (lineCount > MAX_LINES_OF_CODE) {
            LongMethod longMethod(LONG_METHOD, lineCount, function.getName());
            longMethodOccurences.push_back(longMethod);
        }
    }
}

void CodeSmellDetector::detectLongParameterList() {
    for (const Function &function : functionList) {
        int parameterCount = function.getNumberOfParameters();
        if (parameterCount > MAX_PARAMETER_COUNT) {
            LongParameterList longParameterList(LONG_PARAMETER_LIST, parameterCount, function.getName());
            longParameterListOccurences.push_back(longParameterList);
        }
    }
}

void CodeSmellDetector::detectDuplicatedCode() {
    size_t size = functionList.size();
    vector<pair<size_t, size_t>> alreadyCompared;

    for (size_t i = 0; i < size; i++) { // FIXME: don't need to check same function pairs
        for (size_t j = 0; j < size; j++) {
            // Don't compare the same function
            if (i == j) {
                continue;
            }

            pair<size_t, size_t> pair = getSortedPair(i, j);

            // If we already compared these two functions
            if (find(alreadyCompared.begin(), alreadyCompared.end(), pair) != alreadyCompared.end()) {
                continue;
            }

            alreadyCompared.push_back(pair);

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

double CodeSmellDetector::jaccardBiGramSimilarityIndex(string firstCodeString, string secondCodeString) {
    vector<string> firstBigrams;
    vector<string> secondBigrams;

    cout << firstCodeString << " bigrams: " << endl;
    for (int i = 0; i < firstCodeString.size() - 1; i++) {
        stringstream bigram;
        bigram << firstCodeString[i] << firstCodeString[i + 1];
        firstBigrams.push_back(bigram.str());
    }

    cout << secondCodeString << " bigrams: " << endl;
    for (int i = 0; i < secondCodeString.size() - 1; i++) {
        stringstream bigram;
        bigram << secondCodeString[i] << secondCodeString[i + 1];
        secondBigrams.push_back(bigram.str());
    }

    int matchingTokens = 0;
    for (const string &token : firstBigrams) {
        // If token from function one is also in function two
        if (find(secondBigrams.begin(), secondBigrams.end(), token) != secondBigrams.end()) {
            matchingTokens++;
        }
    }

    vector<string> tokensInEither;
    for (const string &token : firstBigrams) {
        tokensInEither.push_back(token);
    }
    for (const string &token : secondBigrams) {
        // If unique token from function two has not been recorded yet
        if (find(tokensInEither.begin(), tokensInEither.end(), token) == tokensInEither.end()) {
            tokensInEither.push_back(token);
        }
    }
    return static_cast<double>(matchingTokens) / static_cast<double>(tokensInEither.size());
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
                tokenCounts.insert({token, 1});
            } else {
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
            // Add token from second function
            allUniqueTokens.insert({token, count});
        } else {
            // Matching token from first and second function so just update the count
            tokenEntry->second += count;
        }
    }

    return allUniqueTokens;
}

vector<string> CodeSmellDetector::getFunctionNames() const {
    return functionNames;
}

vector<CodeSmellDetector::LongParameterList> CodeSmellDetector::getLongParameterListOccurrences() const {
    return longParameterListOccurences;
}

vector<CodeSmellDetector::DuplicatedCode> CodeSmellDetector::getDuplicateCodeOccurrences() const {
    return duplicatedCodeOccurrences;
}

vector<CodeSmellDetector::LongMethod> CodeSmellDetector::getLongMethodOccurrences() const {
    return longMethodOccurences;
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
    return !longMethodOccurences.empty();
}

bool CodeSmellDetector::hasLongParameterListSmell() const {
    return !longParameterListOccurences.empty();
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







