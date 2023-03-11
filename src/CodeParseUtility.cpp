//
// Created by kogge on 3/8/2023.
//

#include "CodeParseUtility.h"
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

const char CodeParseUtility::OPENING_PAREN = '(';
const char CodeParseUtility::CLOSING_PAREN = ')';
const char CodeParseUtility::OPENING_CURLY_BRACKET = '{';
const char CodeParseUtility::CLOSING_CURLY_BRACKET = '}';
const char CodeParseUtility::COMMA = ',';
const char CodeParseUtility::SEMICOLON = ';';
const char CodeParseUtility::WHITESPACE = ' ';
const char CodeParseUtility::FWD_SLASH = '/';
const char CodeParseUtility::POINTER = '*';
const string CodeParseUtility::INCLUDE_DIRECTIVE = "#include";
const string CodeParseUtility::SENTINEL_VAL = "SKIP INDEX 0";
const unordered_map<char, char> CodeParseUtility::BRACKET_MAP = {
        {OPENING_CURLY_BRACKET, CLOSING_CURLY_BRACKET},
        { OPENING_PAREN, CLOSING_PAREN}
};

using namespace std;

CodeParseUtility::CodeParseUtility(const vector<string> &linesFromFile) {
    this->linesFromFile = linesFromFile;
    this->linesFromFile.insert(this->linesFromFile.begin(), SENTINEL_VAL);
    this->fileLineCount = linesFromFile.size();
}

vector<vector<string>> CodeParseUtility::getFunctionContentList() {
    vector<vector<string>> functionContentList;
    size_t currentLineNumber = 1;

    while (currentLineNumber < fileLineCount) {
        skipBlankLines(currentLineNumber);
        skipLinesUntilFunctionHeader(currentLineNumber);
        size_t openParenLineNumber = currentLineNumber;

        skipLinesUntilOpeningCurlyBracket(currentLineNumber);
        size_t openCurlyLineNumber = currentLineNumber;

        size_t endLineNumber = findFunctionClosingCurlyBracketLine(openCurlyLineNumber);

        // Now extract function content
        vector<string> functionContent;
        extractFunctionContent(functionContent, openParenLineNumber, endLineNumber);
        functionContentList.push_back(functionContent);
        currentLineNumber = endLineNumber + 1;
    }

    return functionContentList;
}

void CodeParseUtility::skipBlankLines(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount && isBlankLine(linesFromFile[currentLineNumber])) {
        currentLineNumber++;
    }
}

void CodeParseUtility::skipLinesUntilFunctionHeader(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount && isNotBeginningOfFunctionDefinition(linesFromFile[currentLineNumber])) {
        currentLineNumber++;
    }
}

void CodeParseUtility::skipLinesUntilOpeningCurlyBracket(size_t &currentLineNumber) {
    while (currentLineNumber < fileLineCount &&
           !containsCharacter(linesFromFile[currentLineNumber], OPENING_CURLY_BRACKET)) {
        currentLineNumber++;
    }
}


bool CodeParseUtility::isBlankLine(const string &line) {
    return line.empty() || line == "\r" || line == "\n";
}

void CodeParseUtility::extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber) {
    for (size_t i = startLineNumber; i <= endLineNumber; i++) {
        string line = linesFromFile[i];

        // Ignore blank lines and comments
        if (isBlankLine(line) || isComment(line)) {
            continue;
        }

        functionContent.push_back(line);
    }
}

size_t CodeParseUtility::getClosingBracketIndex(const string &line, const char &openingBracket) {
    size_t startAtZero = 0;
    return CodeParseUtility::getClosingBracketIndex(line, openingBracket, startAtZero);
}

size_t CodeParseUtility::getClosingBracketIndex(const string &line, const char &openingBracket, size_t &openCount) {
    for (size_t index = 0; index < line.size(); index++) {
        char currentChar = line[index];

        if (currentChar == openingBracket) {
            openCount++;
        } else if (currentChar == BRACKET_MAP.at(openingBracket)) {
            if (openCount == 1) {
                // Found initial matching bracket
                return index;
            } else if (openCount > 0) {
                // Found matching bracket but not for the initial opening one
                openCount--;
            }
        } // else skip
    }

    return NOT_FOUND;
}

size_t CodeParseUtility::findFunctionClosingCurlyBracketLine(size_t startLineNumber) {
    size_t openCurlyCount = 0;
    for (size_t currentLineNumber = startLineNumber; currentLineNumber < linesFromFile.size(); currentLineNumber++) {
        size_t closingIndex = getClosingBracketIndex(linesFromFile[currentLineNumber],
                                                     OPENING_CURLY_BRACKET,openCurlyCount);

        if (closingIndex != NOT_FOUND) {
            // Found the closing bracket on the current line number
            return currentLineNumber;
        }
    }

    // Should never reach here assuming input file is valid (compilable) C++
    throw invalid_argument("Failed to find matching curly bracket");
}

bool CodeParseUtility::containsCharacter(const string &str, const char &character) {
    return str.find(character) != string::npos;
}

bool CodeParseUtility::isNotBeginningOfFunctionDefinition(const string &line) {
    return isBlankLine(line) || isComment(line) ||
           line.find(INCLUDE_DIRECTIVE) != string::npos || // if is include directive
           !containsCharacter(line, OPENING_PAREN) ||
           lineEndsWith(line, SEMICOLON); // check forward declarations
}

bool CodeParseUtility::isComment(const string &line) {
    if (line.empty()) {
        return false;
    }

    string strToCompare = line.substr(line.find_first_not_of(WHITESPACE)); // Strip leading whitespace
    return strToCompare[0] == FWD_SLASH || strToCompare[0] == POINTER;
}

bool CodeParseUtility::lineEndsWith(const string &line, const char &character) {
    size_t lastIndex = line.find_last_not_of(" \r\n"); // Ignore whitespace and carriage return
    return line[lastIndex] == character;
}