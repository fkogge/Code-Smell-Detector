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

const size_t Function::FIRST_LINE = 0;
const char Function::OPENING_PAREN = '(';
const char Function::CLOSING_PAREN = ')';
const char Function::COMMA = ',';
const char Function::SPACE = ' ';

Function::Function(vector<string> codeLines) {
    this->codeLines = codeLines;
    this->numLinesOfCode = codeLines.size();
    this->name = extractName();
    this->codeString = transformToCodeString();
    this->numParameters = extractParameterCount();
}

size_t Function::getNumberOfLinesOfCode() const {
    return numLinesOfCode;
}

size_t Function::getNumberOfParameters() const {
    return numParameters;
}

string Function::getName() const {
    return name;
}

string Function::getCodeString() const {
    return codeString;
}

string Function::extractName() {
    string functionHeader = getFunctionHeader();
    istringstream iss(functionHeader);

    string throwawayReturnType;
    iss >> throwawayReturnType;
    string restOfFunctionHeader;
    iss >> restOfFunctionHeader;

    return restOfFunctionHeader.substr(0, restOfFunctionHeader.find(OPENING_PAREN));
}

string Function::transformToCodeString() {
    string codeString = "";
    for (const string &line : codeLines) {
        codeString += line;
    }

    codeString.erase(
            remove(codeString.begin(), codeString.end(), SPACE),
            codeString.end()
            );
    size_t firstCurlyIndex = codeString.find_first_of('{');
    size_t lastCurlyIndex = codeString.find_last_of('}');
    codeString = codeString.substr(firstCurlyIndex + 1, lastCurlyIndex - firstCurlyIndex - 1);
    cout << "Code string, removed curlys: " << codeString << endl;

    return codeString;
}

size_t Function::extractParameterCount() {
    string functionHeader = getFunctionHeader();
    size_t openParenIndex = functionHeader.find(OPENING_PAREN);
    size_t closingParenIndex = functionHeader.find(CLOSING_PAREN);

    // Adjustments by 1 to remove the parentheses
    string paramString = functionHeader.substr(openParenIndex + 1, closingParenIndex - openParenIndex - 1);
    cout << "param string: " << paramString << endl;

    if (paramString.empty()) {
        return 0;
    }

    size_t paramCount = 1;
    for (char c : paramString) {
        if (c == COMMA) {
            paramCount++;
        }
    }
    cout << "param count: " << paramCount << endl;
    return paramCount;

}

string Function::getFunctionHeader() const {
    return codeLines[FIRST_LINE];
}

vector<string> Function::getFunctionBody() const {

    // FIXME: might be doing this wrong
    vector<string> functionBody;
    for (size_t i = 1; i < numLinesOfCode - 1; i++) {
        functionBody.push_back(codeLines[i]);
    }
    return functionBody;
}
