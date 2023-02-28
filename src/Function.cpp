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
const char Function::OPENING_CURLY_BRACKET = '{';
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

    return codeString;
}

size_t Function::extractParameterCount() {
    string functionHeader = getFunctionHeader();
    size_t openParenIndex = functionHeader.find(OPENING_PAREN);
    size_t closingParenIndex = functionHeader.find(CLOSING_PAREN);

    // Adjustments by 1 to remove the parentheses
    string paramString = functionHeader.substr(openParenIndex + 1, closingParenIndex - openParenIndex - 1);

    if (paramString.empty()) {
        return 0;
    }

    size_t paramCount = 1;
    for (char c : paramString) {
        if (c == COMMA) {
            paramCount++;
        }
    }
    return paramCount;

}

string Function::getFunctionHeader() const {
    return codeLines[FIRST_LINE];
}

vector<string> Function::getFunctionBody() const {
    size_t bodyStartLine = (codeLines[FIRST_LINE].find(OPENING_CURLY_BRACKET) != string::npos) ? 1 : 2;

    vector<string> functionBody;

    // If function is defined and written in one line
    if (numLinesOfCode == 1) {
        string line = codeLines[FIRST_LINE];
        size_t openCurlyIndex = line.find_first_of('{');
        size_t closingCurlyIndex = line.find_last_of('}');
        string body = line.substr(openCurlyIndex + 1, closingCurlyIndex - openCurlyIndex - 1);
        functionBody.push_back(body);
    } else {
        for (size_t i = bodyStartLine; i < numLinesOfCode - 1; i++) {
            functionBody.push_back(codeLines[i]);
        }
    }

    return functionBody;
}
