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

string Function::extractName() const {
    const string ampersand = string(1, CodeParseUtility::AMPERSAND);
    const string asterisk = string(1, CodeParseUtility::ASTERISK);
    string functionHeader = getFunctionHeader();
    istringstream iss(functionHeader);

    string throwawayReturnType;
    iss >> throwawayReturnType;

    string next;
    iss >> next;
    // If function is pointer or reference type, get next token
    if (next == ampersand || next == asterisk) {
        iss >> next;
    }

    string restOfFunctionHeader = next;
    return restOfFunctionHeader.substr(0, restOfFunctionHeader.find(CodeParseUtility::OPENING_PAREN));
}

int Function::extractParameterCount() const {
    // Get substring between the parentheses
    string functionHeader = getFunctionHeader();
    size_t leftIndex = functionHeader.find_first_of(CodeParseUtility::OPENING_PAREN);
    size_t rightIndex = functionHeader.find_last_of(CodeParseUtility::CLOSING_PAREN);
    string paramString = functionHeader.substr(leftIndex + 1, rightIndex - leftIndex - 1);

    // If parameter contents is empty
    // or only whitespaces (couldn't find index that isn't a whitespace)
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

string Function::generateCodeString() const {
    ostringstream ss;
    for (const string &line : codeLines) {
        ss << line;
    }
    return ss.str();
}

string Function::getFunctionHeader() const {
    string firstLine = codeLines[FIRST_LINE];

    if (numLinesOfCode > 1) {
        return firstLine;
    } else {
        size_t closingParenIndex = CodeParseUtility::getClosingBracketIndex(firstLine, CodeParseUtility::OPENING_PAREN);
        if (closingParenIndex == CodeParseUtility::NOT_FOUND) {
            throw invalid_argument("Failed to find matching curly bracket");
        }

        return firstLine.substr(0, closingParenIndex + 1);
    }
}
