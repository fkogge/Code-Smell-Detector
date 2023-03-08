//
// Created by Francis Kogge on 2/18/2023.
//

#ifndef CODESMELLDETECTOR_FUNCTION_H
#define CODESMELLDETECTOR_FUNCTION_H


#include <string>
#include <vector>

using namespace std;

/**
 * Represents a function from a code file. Stores important properties of a function
 * including number of lines of code, number of parameters, and name of the function.
 */
class Function {
public:
    // Common tokens
    static const char OPENING_PAREN;
    static const char CLOSING_PAREN;
    static const char OPENING_CURLY_BRACKET;
    static const char CLOSING_CURLY_BRACKET;
    static const char COMMA;
    static const char SEMICOLON;

    /*
     * Initialize all function properties
     */
    explicit Function(const vector<string> &codeLines);

    /*
     * Get the number of lines of code (LOC) in this function
     */
    size_t getNumberOfLinesOfCode() const;

    /*
     * Get number of parameters in this function
     */
    int getNumberOfParameters() const;

    /*
     * Get the name of this function
     */
    string getName() const;

    /*
     * Get the code from the body of the function in the form of a string.
     */
    string getCodeString() const;

private:
    static const size_t FIRST_LINE; // Line 1 stored at index 0

    vector<string> codeLines;
    string name;
    size_t numLinesOfCode;
    int numParameters;
    string codeString;

    string extractName();
    int extractParameterCount();
    string getFunctionHeader() const;
    string generateCodeString();

    // Helper method mainly for getting code between common matching
    // tokens such as parentheses or curly brackets
    static string getSubstringBetweenCharacters(const string &line, const char &left, const char &right);
};


#endif //CODESMELLDETECTOR_FUNCTION_H
