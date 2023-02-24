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
using namespace std;

const char CodeSmellDetector::OPENING_CURLY_BRACKET = '{';
const char CodeSmellDetector::CLOSING_CURLY_BRACKET = '}';
const char CodeSmellDetector::OPENING_PAREN = '(';


CodeSmellDetector::CodeSmellDetector(const string &fileName, const vector<string> &linesFromFile) {
    this->fileName = fileName;
    this->linesFromFile = linesFromFile;
    this->linesFromFile.insert(this->linesFromFile.begin(), "SKIP INDEX 0");
    this->lineCount = (int) linesFromFile.size();

    extractFunctions();
    detectLongMethod();
    detectLongParameterList();
    detectDuplicatedCode();

    for (const Function &function : this->functionList) {
        this->functionNames.push_back(function.getName());
    }
}

void CodeSmellDetector::extractFunctions() {
    int currentLineNumber = 1;
    int openParenLineNumber = 1;
    int openCurlyLineNumber = 1;

    while (currentLineNumber < lineCount) {
        skipBlankLines(currentLineNumber);

        // Skip lines till we reach a function header which has an open paren
        skipLinesUntilFunctionHeader(currentLineNumber);
        openParenLineNumber = currentLineNumber;

        // Skip lines till we reach the line with the opening curly bracket
        skipLinesUntilOpeningCurlyBracket(currentLineNumber);
        openCurlyLineNumber = currentLineNumber;

        cout << "paren: " << openParenLineNumber;
        cout << "curly: " << openCurlyLineNumber;

        string line = linesFromFile[currentLineNumber];
        int endLineNumber = findFunctionClosingCurlyBracket(line, openCurlyLineNumber);

        cout << "\nstart line number: " << openParenLineNumber << endl;
        cout << "end line number: " << endLineNumber << endl;

        vector<string> functionContent;
        extractFunctionContent(functionContent, openParenLineNumber, endLineNumber);

        Function function(functionContent, openParenLineNumber, openCurlyLineNumber, endLineNumber);
        functionList.push_back(function);

        currentLineNumber = (openParenLineNumber == endLineNumber) ? endLineNumber + 1 : endLineNumber;
    }
}

void CodeSmellDetector::skipBlankLines(int &currentLineNumber) {
    while (currentLineNumber < lineCount && linesFromFile[currentLineNumber].empty()) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilFunctionHeader(int &currentLineNumber) {
    while (currentLineNumber < lineCount &&
           !containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_PAREN)) {
        currentLineNumber++;
    }
}

void CodeSmellDetector::skipLinesUntilOpeningCurlyBracket(int &currentLineNumber) {
    while (currentLineNumber < lineCount &&
           !containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_CURLY_BRACKET)) {
        currentLineNumber++;
    }
}

bool CodeSmellDetector::containsCharacter(const string &str, const char &character) {
    return str.find(character) != string::npos;
}

int CodeSmellDetector::findFunctionClosingCurlyBracket(const string &startLine, int lineNumber) {
    stack<char> openCurlyBrackets;

    for (size_t i = lineNumber; i < this->linesFromFile.size(); i++) {
        for (const char &currentChar : this->linesFromFile[i]) {
            if (currentChar == CodeSmellDetector::OPENING_CURLY_BRACKET) {
                openCurlyBrackets.push(currentChar);
            } else if (currentChar == CodeSmellDetector::CLOSING_CURLY_BRACKET) {
                if (openCurlyBrackets.size() == 1) {
                    return i;
                } else if (!openCurlyBrackets.empty()) {
                    openCurlyBrackets.pop();
                }
            }
        }
    }

    return -1;
}

void CodeSmellDetector::extractFunctionContent(vector<string> &functionContent, int startLineNumber, int endLineNumber) {
    for (int i = startLineNumber; i <= endLineNumber; i++) {
        functionContent.push_back(linesFromFile[i]);
        cout << linesFromFile[i] << endl;
    }
}

void CodeSmellDetector::detectLongMethod() {
    for (const Function &function : functionList) {
        int lineCount = function.getNumberOfLinesOfCode();
        if (lineCount > MAX_LINES_OF_CODE) {
            LongMethodOccurrence longMethod(LONG_METHOD, lineCount, function.getName());
            longMethodOccurences.push_back(longMethod);
        }
    }
}

void CodeSmellDetector::detectLongParameterList() {
    for (const Function &function : functionList) {
        int parameterCount = function.getNumberOfParameters();
        if (parameterCount > MAX_PARAMETER_COUNT) {
            LongParameterListOccurrence longParameterList(LONG_PARAMETER_LIST, parameterCount, function.getName());
            longParameterListOccurences.push_back(longParameterList);
        }
    }
}

void CodeSmellDetector::detectDuplicatedCode() {
    int size = (int) functionList.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1; j++) {
            if (j == i) {
                continue;
            }

            string functionOneString = functionList[i].getCodeString();
            string functionTwoString = functionList[j].getCodeString();
            double similarityIndex = calculateSimilarityIndex(functionOneString, functionTwoString);
            cout << "similarity index for " << functionList[i].getName() << " and " << functionList[j].getName() << ": " << similarityIndex << endl;
            if (similarityIndex > MAX_SIMILARITY_INDEX) {
                DuplicateCodeOccurrence duplicatedCode(DUPLICATED_CODE, similarityIndex, functionOneString, functionTwoString);
                duplicatedCodeOccurrences.push_back(duplicatedCode);
            }
        }
    }
}

double CodeSmellDetector::calculateSimilarityIndex(string stringOne, string stringTwo) {
//    double similarityIndex;
    bool characters[POSSIBLE_CHARS];
    bool strOneCharacters[POSSIBLE_CHARS];
    bool strTwoCharacters[POSSIBLE_CHARS];
    for (int i = 0; i < POSSIBLE_CHARS; i++) {
        characters[i] = false;
        strOneCharacters[i] = false;
        strTwoCharacters[i] = false;
    }



    for (unsigned char currentChar : stringOne) {
        characters[currentChar] = true;
        strOneCharacters[currentChar] = true;
    }
    for (unsigned char currentChar : stringTwo) {
        characters[currentChar] = true;
        strTwoCharacters[currentChar] = true;
    }

    int totalCharactersSeen = 0;
    for (bool character : characters) {
        if (character) {
            totalCharactersSeen++;
        }
    }

    int commonCharacters = 0;
    for (int i = 0; i < POSSIBLE_CHARS; i++) {
        if (strOneCharacters[i] && strTwoCharacters[i]) {
            commonCharacters++;
        }
    }
    cout << "common: " << commonCharacters << endl;
    cout << "total: " << totalCharactersSeen << endl;
    cout << "similarity index: " << (double) commonCharacters / totalCharactersSeen << endl;
    return (double) commonCharacters / totalCharactersSeen;
}

vector<string> CodeSmellDetector::getFunctionNames() {
    return functionNames;
}

vector<CodeSmellDetector::LongParameterListOccurrence> CodeSmellDetector::getLongParameterListOccurrences() const {
    return longParameterListOccurences;
}

vector<CodeSmellDetector::DuplicateCodeOccurrence> CodeSmellDetector::getDuplicateCodeOccurrences() const {
    return duplicatedCodeOccurrences;
}

vector<CodeSmellDetector::LongMethodOccurrence> CodeSmellDetector::getLongMethodOccurrences() const {
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
