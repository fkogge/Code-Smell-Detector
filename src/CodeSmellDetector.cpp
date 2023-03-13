//
// Created by Francis Kogge on 2/17/2023.
//

#include "CodeSmellDetector.h"
#include "Function.h"
#include <vector>
#include <climits>
#include <algorithm>
#include <unordered_set>
#include "Parser.h"

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
    Parser parser(linesFromFile);
    vector<vector<string>> functionContentList = parser.getFunctionContentList();

    for (const vector<string> &content : functionContentList) {
        Function function(content);
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

            double similarityIndex = jaccardSimilarityIndex(firstFunction.getCodeString(),
                                                            secondFunction.getCodeString());

            if (similarityIndex > MAX_SIMILARITY_INDEX) {
                DuplicatedCode duplicatedCode(DUPLICATED_CODE, similarityIndex,
                                              firstFunction.getName(),secondFunction.getName());
                duplicatedCodeOccurrences.push_back(duplicatedCode);
            }
        }
    }
}

double CodeSmellDetector::jaccardSimilarityIndex(const string &firstCodeString, const string &secondCodeString) {
    unordered_set<char> firstChars;
    unordered_set<char> secondChars;

    // Get char sets for each function
    fillCharSet(firstChars, firstCodeString);
    fillCharSet(secondChars, secondCodeString);

    // Intersection of chars across both functions
    size_t matchingChars = 0;
    for (const char &currentChar : firstChars) {
        // If bigram from first function is also a bigram in second function
        if (secondChars.find(currentChar) != secondChars.end()) {
            matchingChars++;
        }
    }

    // All unique chars in either function (we can initialize using number of unique
    // chars from first function since either set is a subset of the union set)
    size_t totalUniqueChars = firstChars.size();
    for (const char &currentChar : secondChars) {
        // If we haven't recorded unique char from second function yet
        if (firstChars.find(currentChar) == firstChars.end()) {
            totalUniqueChars++;
        }
    }

    return static_cast<double>(matchingChars) / static_cast<double>(totalUniqueChars);
}

void CodeSmellDetector::fillCharSet(unordered_set<char> &charSet, const string &codeString) {
    for (char c : codeString) {
        charSet.insert(c);
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