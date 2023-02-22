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
    this->name = extractName();
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

string Function::extractName() {
    string name = "";
    string functionHeader = getFunctionHeader();
    istringstream iss(functionHeader);

    string throwawayReturnType;
    iss >> throwawayReturnType;
    string restOfFunctionHeader;
    iss >> restOfFunctionHeader;
    name = restOfFunctionHeader.substr(0, restOfFunctionHeader.find('('));
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
    string functionHeader = getFunctionHeader();
    size_t openParenIndex = functionHeader.find('(');
    size_t closingParenIndex = functionHeader.find(')');

    // Adjustments by 1 to remove the parentheses
    string paramString = functionHeader.substr(openParenIndex + 1, closingParenIndex - openParenIndex - 1);
    cout << "param string: " << paramString << endl;

    if (paramString.empty()) {
        return 0;
    }

    int paramCount = 1;
    for (char c : paramString) {
        if (c == ',') {
            paramCount++;
        }
    }
    cout << "param count: " << paramCount << endl;
    return paramCount;

}

string Function::getFunctionHeader() const {
    return codeLines[0];
}