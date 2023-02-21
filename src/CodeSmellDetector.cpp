//
// Created by Francis Kogge on 2/17/2023.
//

#include "CodeSmellDetector.h"
#include "Function.h"
#include "Function.cpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <stack>
#include <unordered_set>
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
        if (lineCount > MAX_PARAMETER_COUNT) {
            LongParameterListOccurrence longParameterList(LONG_PARAMETER_LIST, parameterCount, function.getName());
            longParameterListOccurences.push_back(longParameterList);
        }
    }
}

void CodeSmellDetector::detectDuplicatedCode() {
    int size = (int) functionList.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1; j++) {
            if (j == i) continue;
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

bool CodeSmellDetector::containsCharacter(string str, const char &character) {
    return str.find(character) != string::npos;
}

int CodeSmellDetector::findLineNumberWithClosingCurlyBracket(const string &startLine, int lineNumber) {
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

void CodeSmellDetector::extractFunctions() {
    int currentLineNumber = 1;
    int parenOpenLineNumber = 1;
    int curlyOpenLineNumber = 1;



    while (currentLineNumber < lineCount) {
        while (currentLineNumber < lineCount && (linesFromFile[currentLineNumber].empty())) {
            currentLineNumber++;
        }
//        while (currentLineNumber < lineCount &&
//                 linesFromFile[currentLineNumber].empty() ||
//                (!containsCharacter(linesFromFile[currentLineNumber], '/') ||
//                 !containsCharacter(linesFromFile[currentLineNumber], '*')) ) {
//            currentLineNumber++;
//        }


        while (currentLineNumber < lineCount &&
               !containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_PAREN)) {
            currentLineNumber++;
        }
        parenOpenLineNumber = currentLineNumber;

        while (currentLineNumber < lineCount &&
               !containsCharacter(linesFromFile[currentLineNumber], CodeSmellDetector::OPENING_CURLY_BRACKET)) {
            currentLineNumber++;
        }
        curlyOpenLineNumber = currentLineNumber;
        cout << "paren: " << parenOpenLineNumber;
        cout << "curly: " << curlyOpenLineNumber;


        string line = linesFromFile[currentLineNumber];
        int endLineNumber = findLineNumberWithClosingCurlyBracket(line, curlyOpenLineNumber);
        cout << "\nstart line number: " << parenOpenLineNumber << endl;
        cout << "end line number: " << endLineNumber << endl;

        vector<string> functionContent;
        for (int i = parenOpenLineNumber; i <= endLineNumber; i++) {
            functionContent.push_back(linesFromFile[i]);
            cout << linesFromFile[i] << endl;
        }
        Function function(functionContent, parenOpenLineNumber, curlyOpenLineNumber, endLineNumber);
        functionList.push_back(function);

        currentLineNumber = (parenOpenLineNumber == endLineNumber) ? endLineNumber + 1 : endLineNumber;
    }


//    //////////////////////////////////////
//    struct StartEndPair {
//        int startLine;
//        int endLine;
//        StartEndPair(int startLine, int endLine) {
//            this->startLine = startLine;
//            this->endLine = endLine;
//        }
//    };
//
//    struct Line {
//        vector<string> tokens;
//        string line;
//        int number;
//    };
//
//    vector<StartEndPair> startEndPairs;
//
//
//
//    for (int startLineNumber = 1; startLineNumber < linesFromFile.size(); startLineNumber++) {
//        string startLine = linesFromFile[startLineNumber];
//        vector<string> tokens;
//        istringstream iss(startLine);
//        string token;
//
//        Line line;
//        line.line = startLine;
//        line.number = startLineNumber;
//
//        while (iss >> token) {
//            line.tokens.push_back(token);
//            if (containsCharacter(token, OPENING_CURLY_BRACKET)) {
//                int endLineNumber = findLineNumberWithClosingCurlyBracket(startLine, startLineNumber);
//                StartEndPair pair = StartEndPair(startLineNumber, endLineNumber);
//                startEndPairs.push_back(pair);
//                cout << "\nstart line number: " << startLineNumber << endl;
//                cout << "end line number: " << endLineNumber << endl;
//
//                // Not a bracket enclosed statement like if, loop, etc.
//                if (BRACKET_ENCLOSED_STATEMENTS.find(line.tokens[0]) == BRACKET_ENCLOSED_STATEMENTS.end()) {
//                    vector<string> functionContent;
//                    for (int i = startLineNumber; i <= endLineNumber; i++) {
//                        functionContent.push_back(linesFromFile[i]);
//                        cout << linesFromFile[i] << endl;
//                    }
//                    Function function(functionContent, "function", endLineNumber - startLineNumber);
//
//                }
//            }
//            cout << startLineNumber << " TOKEN:[" << token << "] ";
//        }
//        cout << endl;
//    }

//    int maxDiff = INT_MIN;
//    //int functionStartLine = -1;
//   // int functionEndLine = -1;
//    for (StartEndPair pair : startEndPairs) {
//        int diff = pair.startLine - pair.endLine;
//        if (diff > maxDiff) {
//            diff = maxDiff;
//            functionStartLine = pair.startLine;
//            functionEndLine = pair.endLine;
//        }
//    }




}

vector<string> CodeSmellDetector::getFunctionNames() {
    return functionNames;
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
