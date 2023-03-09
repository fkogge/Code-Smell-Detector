//
// Created by kogge on 3/8/2023.
//

#ifndef CODESMELLDETECTOR_CODEPARSEUTILITY_H
#define CODESMELLDETECTOR_CODEPARSEUTILITY_H

#include <string>
#include <vector>
using namespace std;

/**
 * Utility class for complex code parsing.
 */
class CodeParseUtility {
public:
    // Common C++ tokens
    static const char OPENING_PAREN;
    static const char CLOSING_PAREN;
    static const char OPENING_CURLY_BRACKET;
    static const char CLOSING_CURLY_BRACKET;
    static const char COMMA;
    static const char SEMICOLON;
    static const char WHITESPACE;
    static const char FWD_SLASH;
    static const char POINTER;
    static const string INCLUDE_DIRECTIVE;

    static const size_t NOT_FOUND = SIZE_MAX;

    explicit CodeParseUtility(const vector<string> &linesFromFile);

    /*
     * Get the index of the matching closing bracket. Defaults the count of the open brackets to 0.
     */
    static size_t getClosingIndex(const string &line, const char &openingChar, const char &closingChar);

    /*
     * Get the index of the matching closing bracket, while using the count of the open brackets passed in
     * by the client (passed by reference).
     */
    static size_t getClosingIndex(const string &line, const char &openingChar, const char &closingChar, size_t &openCount);

    vector<vector<string>> getFunctionContentList();

private:
    size_t fileLineCount;
    vector<string> linesFromFile;

    // Skip lines while updating currentLineNumber (passed by reference)
    void skipBlankLines(size_t &currentLineNumber);
    void skipLinesUntilFunctionHeader(size_t &currentLineNumber);
    void skipLinesUntilOpeningCurlyBracket(size_t &currentLineNumber);

    // Helper functions for checking characteristics of the line of code
    static bool lineEndsWith(const string &line, const char &character);
    static bool isComment(const string &line);
    static bool isNotBeginningOfFunctionDefinition(const string &line);
    static bool isBlankLine(const string &line);
    static bool containsCharacter(const string &str, const char &character);

    size_t findFunctionClosingCurlyBracketLine(size_t startLineNumber);
    void extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber);
};


#endif //CODESMELLDETECTOR_CODEPARSEUTILITY_H
