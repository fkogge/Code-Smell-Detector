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

using namespace std;

const string CodeSmellDetector::INCLUDE_DIRECTIVE = "#include";

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
    while (currentLineNumber < fileLineCount && isBlankLine(linesFromFile[currentLineNumber])) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilFunctionHeader(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount && !isBeginningOfFunctionDefinition(linesFromFile[currentLineNumber])) {
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
        if (isBlankLine(line)) {
            continue;
        }

        functionContent.push_back(line);
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
    size_t totalUniqueBigrams = firstBigrams.size(); // either function's bigrams is a subset
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

bool CodeSmellDetector::isBlankLine(const string &line) {
    return line.empty() || line == "\r" || line == "\n";
}

bool CodeSmellDetector::lineEndsWith(const string &line, const char &character) {
    size_t lastIndex = line.find_last_not_of(" \r\n"); // Ignore whitespace and carriage return
    return line[lastIndex] == character;
}

bool CodeSmellDetector::isBeginningOfFunctionDefinition(const string &line) {
    return !isBlankLine(line) &&
        line.find(INCLUDE_DIRECTIVE) == string::npos && // is not include directive
        containsCharacter(line, Function::OPENING_PAREN) &&
        !lineEndsWith(line, Function::SEMICOLON);
}


