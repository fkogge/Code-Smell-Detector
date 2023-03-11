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
    /**
     * Initialize all function properties
     * @param codeLines lines of code that comprise the function
     */
    explicit Function(const vector<string> &codeLines);

    /**
     * Get the number of lines of code (LOC) in this function
     * @return number of LOC
     */
    size_t getNumberOfLinesOfCode() const;

    /**
     * Get number of parameters in this function
     * @return number of parameters
     */
    int getNumberOfParameters() const;

    /**
     * Get the name of this function
     * @return name of the function
     */
    string getName() const;

    /**
     * Get the code from the body of the function in the form of a string. Mashes
     * the function into one giant string.
     * @return string representation of the function code
     */
    string getCodeString() const;

private:
    static const size_t FIRST_LINE = 0; // Line 1 stored at index 0

    vector<string> codeLines;
    string name;
    size_t numLinesOfCode;
    int numParameters;
    string codeString;

    // Helper methods for parsing different parts of the function
    string extractName() const;
    int extractParameterCount() const;
    string getFunctionHeader() const;
    string generateCodeString() const;
};


#endif //CODESMELLDETECTOR_FUNCTION_H
