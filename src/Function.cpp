//
// Created by kogge on 2/18/2023.
//

#include "Function.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

Function::Function(vector<string> codeLines, int startLineNumber, int endDefLineNumber, int endLineNumber) {
    this->codeLines = codeLines;
    this->numLinesOfCode = endLineNumber - startLineNumber;
    this->name = extractName(startLineNumber, endDefLineNumber);
    this->codeString = transformToCodeString();
    this->numParameters = extractParameterCount();
}

int Function::getNumberOfLinesOfCode() const {
    return numLinesOfCode;
}

int Function::getNumberOfParameters() const {
    return numParameters;
}

string Function::getName() const {
    return name;
}

string Function::getCodeString() const {
    return codeString;
}

string Function::extractName(int startLineNumber, int endDefLineNumber) {
    string name = "";
    string firstLine = codeLines[0];
    string openingCurlyBracketLine = codeLines[endDefLineNumber - startLineNumber];
    //cout << firstLine << endl << openingCurlyBracketLine << endl;

    istringstream iss(firstLine);

    string throwaway;
    iss >> throwaway;

    string restOfFirstLine;
    iss >> restOfFirstLine;
    //cout << restOfFirstLine << endl;

    name = restOfFirstLine.substr(0, restOfFirstLine.find('('));
    cout << "function name: " << name << endl;
    return name;
}

string Function::transformToCodeString() {
    string codeString = "";
    for (const string &line : codeLines) {
        codeString += line;
    }
    codeString.erase(remove(codeString.begin(), codeString.end(), ' '), codeString.end());
    cout << "code string size: " << codeString.size() << endl;
    cout << "code string: " << codeString << endl;
    return codeString;
}

int Function::extractParameterCount() {
    return 0;
}
