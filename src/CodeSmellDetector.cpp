//
// Created by Francis Kogge on 2/17/2023.
//

#include "CodeSmellDetector.h"
#include "Function.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <stack>
#include <climits>
#include <algorithm>
#include <unordered_map>
using namespace std;

const char CodeSmellDetector::OPENING_CURLY_BRACKET = '{';
const char CodeSmellDetector::CLOSING_CURLY_BRACKET = '}';
const char CodeSmellDetector::OPENING_PAREN = '(';


CodeSmellDetector::CodeSmellDetector(const string &fileName, const vector<string> &linesFromFile) {
    this->fileName = fileName;
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
           !containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_PAREN)) {
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
    stack<char> openCurlyBrackets;

    for (size_t currentLineNumber = startLineNumber; currentLineNumber < linesFromFile.size(); currentLineNumber++) {
        for (const char &currentChar : linesFromFile[currentLineNumber]) {
            if (currentChar == CodeSmellDetector::OPENING_CURLY_BRACKET) {
                openCurlyBrackets.push(currentChar);
            } else if (currentChar == CodeSmellDetector::CLOSING_CURLY_BRACKET) {
                if (openCurlyBrackets.size() == 1) {
                    return currentLineNumber;
                } else if (!openCurlyBrackets.empty()) {
                    openCurlyBrackets.pop();
                }
            }
        }
    }

    return -1;
}

void CodeSmellDetector::extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber) {
    for (size_t i = startLineNumber; i <= endLineNumber; i++) {
        string line = linesFromFile[i];
        if (line.empty() || line == "\n" || line == "\r") {
            continue;
        }
        functionContent.push_back(linesFromFile[i]);
        cout << linesFromFile[i] << endl;
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
            vector<string> firstFuncBody = firstFunction.getFunctionBody();
            vector<string> secondFuncBody = secondFunction.getFunctionBody();
            cout << "comparing " << firstFunction.getName() << " and " << secondFunction.getName() << endl;
            double similarityIndex = jaccardTokenSimilarityIndex(firstFuncBody, secondFuncBody);
            //double similarityIndex = jaccardBiGramSimilarityIndex(firstFunction.getCodeString(), secondFunction.getCodeString());
            //double similarityIndex = jaccardLineSimilarityIndex(firstFuncBody, secondFuncBody);
            //double similarityIndex = jaccardSimilarityIndex(firstFunction.getCodeString(), secondFunction.getCodeString());

//            cout << "similarity index for " << functionOneName << " and " << functionTwoName << ": " << similarityIndex << endl;

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

double CodeSmellDetector::jaccardTokenSimilarityIndex(vector<string> firstFunctionBody, vector<string> secondFunctionBody) {
    unordered_map<string, int> firstUniqueTokens;
    unordered_map<string, int> secondUniqueTokens;
    unordered_map<string, int> tokensInBoth;


    int matchingTokens = 0;
    int totalTokens = 0;

    computeFunctionTokenCounts(firstFunctionBody, firstUniqueTokens);

    cout << "first function unique tokens: " << endl;
    for (const auto& entry : firstUniqueTokens) {
        cout << "{" << entry.first << " : " << entry.second << "}" << endl;
    }

    computeFunctionTokenCounts(secondFunctionBody, secondUniqueTokens);

    cout << "second function unique tokens: " << endl;
    for (const auto& entry : secondUniqueTokens) {
        cout << "{" << entry.first << " : " << entry.second << "}" << endl;
    }

    unordered_map<string, int> allUniqueTokens = getAllUniqueTokenCounts(firstUniqueTokens, secondUniqueTokens);

    cout << "all unique tokens: " << endl;
    for (const auto& entry : allUniqueTokens) {
        cout << "{" << entry.first << " : " << entry.second << "}" << endl;
    }

    // observations in both
    for (const auto &firstTokenEntry : firstUniqueTokens) {
        auto matchingTokenEntry = secondUniqueTokens.find(firstTokenEntry.first);
        // if found matching entry
        if (matchingTokenEntry != secondUniqueTokens.end()) {
            tokensInBoth.insert({firstTokenEntry.first, firstTokenEntry.second + matchingTokenEntry->second});
            matchingTokens += firstTokenEntry.second + matchingTokenEntry->second;
        }
    }
    cout << "token in both (intersection): " << endl;
    for (const auto& entry : tokensInBoth) {
        cout << "{" << entry.first << " : " << entry.second << "}" << endl;
    }
    // observations in either
    for (const auto &tokenEntry : allUniqueTokens) {
        totalTokens += tokenEntry.second;
    }

    double simIndex = (double) matchingTokens / totalTokens;
    cout << "sim index: " << matchingTokens << " / " << totalTokens << " = " << simIndex << endl;
    return (double) matchingTokens / totalTokens;
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
//                int count = secondUniqueTokens.at(token);
//                cout << "count: " << count << endl;
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



double CodeSmellDetector::jaccardSimilarityIndex(string firstCodeString, string secondCodeString) {
    unordered_set<char> firstUniqueChars;
    unordered_set<char> secondUniqueChars;
    unordered_set<char> allUniqueChars;
    int intersection = 0;
    cout << "First code: " << firstCodeString << endl;
    for (const char &c : firstCodeString) {
        firstUniqueChars.insert(c);
        allUniqueChars.insert(c);
    }
    for (const char &c : secondCodeString) {
        secondUniqueChars.insert(c);
        allUniqueChars.insert(c);
    }

    cout << "intersection: " << endl;
    for (const char &c : firstUniqueChars) {
        if (secondUniqueChars.find(c) != secondUniqueChars.end()) {
            intersection++;
            cout << c;
        }
    }
    cout << endl;

    cout << "First unique: " << endl;
    for (const char &c : firstUniqueChars) {
        cout << c << ", ";
    }
    cout << endl;

    cout << "second unique: " << endl;
    for (const char &c : secondUniqueChars) {
        cout << c << ", ";
    }
    cout << endl;

    cout << "all unique: " << endl;
    for (const char &c : allUniqueChars) {
        cout << c << ", ";
    }
    cout << endl;

    return (double) intersection / allUniqueChars.size();


}

double CodeSmellDetector::jaccardBiGramSimilarityIndex(string firstCodeString, string secondCodeString) {
    vector<string> firstBigrams;
    vector<string> secondBigrams;

    // FIXME: ignore carriage returns?
    cout << firstCodeString << " bigrams: " << endl;
    for (int i = 0; i < firstCodeString.size() - 1; i++) {
        stringstream bigram;
        bigram << firstCodeString[i] << firstCodeString[i + 1];
        firstBigrams.push_back(bigram.str());
        cout << firstBigrams[i] << endl;
    }

    cout << secondCodeString << " bigrams: " << endl;
    for (int i = 0; i < secondCodeString.size() - 1; i++) {
        stringstream bigram;
        bigram << secondCodeString[i] << secondCodeString[i + 1];
        secondBigrams.push_back(bigram.str());
        cout << secondBigrams[i] << " " << endl;
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
    cout << "matching: " << matchingTokens << endl;
    cout << "tokens in either: " << tokensInEither.size() << endl;
    cout << "sim index: " << static_cast<double>(matchingTokens) / static_cast<double>(tokensInEither.size()) << endl;
    return static_cast<double>(matchingTokens) / static_cast<double>(tokensInEither.size());
}

double CodeSmellDetector::jaccardLineSimilarityIndex(vector<string> functionOneBody, vector<string> functionTwoBody) {
    int matchingTokens = 0;
    for (const string &token : functionOneBody) {
        // If token from function one is also in function two
        if (find(functionTwoBody.begin(), functionTwoBody.end(), token) != functionTwoBody.end()) {
            matchingTokens++;
        }
    }

    vector<string> tokensInEither;
    for (const string &token : functionOneBody) {
        tokensInEither.push_back(token);
    }
    for (const string &token : functionTwoBody) {
        // If unique token from function two has not been recorded yet
        if (find(tokensInEither.begin(), tokensInEither.end(), token) == tokensInEither.end()) {
            tokensInEither.push_back(token);
        }
    }
    cout << "matching: " << matchingTokens << endl;
    cout << "tokens in either: " << tokensInEither.size() << endl;
    cout << "sim index: " << static_cast<double>(matchingTokens) / static_cast<double>(tokensInEither.size()) << endl;
    return static_cast<double>(matchingTokens) / static_cast<double>(tokensInEither.size());
}

vector<string> CodeSmellDetector::getFunctionNames() {
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

bool CodeSmellDetector::hasSmell(CodeSmellDetector::SmellType smellType) const {
    if (smellType == SmellType::LONG_METHOD) {
        return !longMethodOccurences.empty();
    } else if (smellType == SmellType::LONG_PARAMETER_LIST) {
        return !longParameterListOccurences.empty();
    } else if (smellType == SmellType::DUPLICATED_CODE) {
        return !duplicatedCodeOccurrences.empty();
    } else {
        throw invalid_argument("Unknown smell type");
    }

}

pair<size_t, size_t> CodeSmellDetector::getSortedPair(size_t first, size_t second) {
    pair<size_t, size_t> pair(first, second);
    if (first > second) {
        swap(pair.first, pair.second);
    }
    return pair;
}







