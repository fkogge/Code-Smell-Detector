//
// Created by Francis Kogge on 2/18/2023.
//

#include "Function.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "CodeParseUtility.h"
#include <stdexcept>

using namespace std;

Function::Function(const vector<string> &codeLines) {
    this->codeLines = codeLines;
    this->numLinesOfCode = codeLines.size();
    this->name = extractName();
    this->numParameters = extractParameterCount();
    this->codeString = generateCodeString();
}

size_t Function::getNumberOfLinesOfCode() const {
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
    string functionHeader = getFunctionHeader();
    istringstream iss(functionHeader);

    string throwawayReturnType;
    iss >> throwawayReturnType;

    string next;
    iss >> next;
    // If function is pointer or reference type, get next token
    if (next == "*" || next == "&") {
        iss >> next;
    }

    string restOfFunctionHeader = next;
    return restOfFunctionHeader.substr(0, restOfFunctionHeader.find(CodeParseUtility::OPENING_PAREN));
}

int Function::extractParameterCount() {
    string paramString = getSubstringBetweenCharacters(
            getFunctionHeader(),CodeParseUtility::OPENING_PAREN,CodeParseUtility::CLOSING_PAREN);
    // If parameter contents is empty or only whitespaces (couldn't find index that isn't a whitespace)
    if (paramString.empty() || paramString.find_first_not_of(CodeParseUtility::WHITESPACE) == string::npos) {
        return 0;
    }

    int paramCount = 1;
    for (char c : paramString) {
        if (c == CodeParseUtility::COMMA) {
            paramCount++;
        }
    }

    return paramCount;
}

string Function::generateCodeString() {
    ostringstream ss;
    for (const string &line : codeLines) {
        ss << line;
    }
    return getSubstringBetweenCharacters(
            ss.str(),CodeParseUtility::OPENING_CURLY_BRACKET,CodeParseUtility:: CLOSING_CURLY_BRACKET);
}

string Function::getFunctionHeader() const {
    string firstLine = codeLines[FIRST_LINE];

    if (numLinesOfCode > 1) {
        return codeLines[FIRST_LINE];
    } else {
        size_t closingParenIndex = CodeParseUtility::getClosingIndex(
                firstLine, CodeParseUtility::OPENING_PAREN, CodeParseUtility::CLOSING_PAREN);
        if (closingParenIndex == CodeParseUtility::NOT_FOUND) {
            throw invalid_argument("Failed to find matching curly bracket");
        }

        return firstLine.substr(0, closingParenIndex + 1);
    }
}

string Function::getSubstringBetweenCharacters(const string &line, const char &left, const char &right) {
    size_t leftIndex = line.find_first_of(left);
    size_t rightIndex = line.find_last_of(right);
    return line.substr(leftIndex + 1, rightIndex - leftIndex - 1);
}
