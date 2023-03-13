//
// Created by kogge on 3/8/2023.
//

#ifndef CODESMELLDETECTOR_CODEPARSEUTILITY_H
#define CODESMELLDETECTOR_CODEPARSEUTILITY_H

#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

/**
 * Utility class for complex code parsing.
 */
class CodeParseUtility {
public:
    // Common C++ tokens
    static const char OPENING_PAREN, CLOSING_PAREN,
        OPENING_CURLY_BRACKET, CLOSING_CURLY_BRACKET,
        COMMA, SEMICOLON, WHITESPACE, FWD_SLASH,
        ASTERISK, AMPERSAND;

    static const string INCLUDE_DIRECTIVE;
    static const size_t NOT_FOUND = SIZE_MAX;
    static const string SENTINEL_VAL; // Using one based indexing to match line numbers
    static const unordered_map<char, char> BRACKET_MAP; // Match opening brackets to their closing brackets

    /**
     * Initialize the line count and file lines list.
     * @param linesFromFile lines of code from the input file
     */
    explicit CodeParseUtility(const vector<string> &linesFromFile);

    /**
     * Get the index of the matching closing bracket. Defaults the count of the open brackets to 0.
     * @param line line of code
     * @param openingBracket opening bracket to match
     * @return index where matching closing bracket resides
     */
    static size_t getClosingBracketIndex(const string &line, const char &openingBracket);

    /**
     * Stores each line of code from a function into a list. That list is then stored in
     * another list which contains all of the function content lists.
     * @return vector of function content vectors (2D vector)
     */
    vector<vector<string>> getFunctionContentList();

private:
    size_t fileLineCount;
    vector<string> linesFromFile;

    // Extract each line of code from the function and store in the content vector
    void extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber);

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

    /*
     * Get the index of the matching closing bracket, while using the count of the open brackets passed in
     * by the client (passed by reference). This works similar to using a stack to keep track of the open
     * brackets that haven't been matched yet, but we don't actually need a stack and can just keep
     * track of the count instead.
     */
    static size_t getClosingBracketIndex(const string &line, const char &openingBracket, size_t &openCount);

    // This just finds the closing bracket index, but returns the line number it was found on instead.
    size_t findFunctionClosingCurlyBracketLine(size_t startLineNumber);
};


#endif //CODESMELLDETECTOR_CODEPARSEUTILITY_H
