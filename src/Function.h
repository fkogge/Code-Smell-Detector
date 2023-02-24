//
// Created by kogge on 2/18/2023.
//

#ifndef CODESMELLDETECTOR_FUNCTION_H
#define CODESMELLDETECTOR_FUNCTION_H


#include <string>
#include <vector>
using namespace std;

class Function {
public:
    Function(vector<string> codeLines, size_t startLineNumber, size_t endDefLineNumber, size_t endLineNumber);
    size_t getNumberOfLinesOfCode() const;
    size_t getNumberOfParameters() const;
    string getName() const;
    string getCodeString() const;

private:
    static const size_t FIRST_LINE;

    vector<string> codeLines;
    string name;
    size_t numLinesOfCode;
    size_t numParameters;
    string codeString;
    string extractName();
    string transformToCodeString();
    size_t extractParameterCount();
    string getFunctionHeader() const;

};


#endif //CODESMELLDETECTOR_FUNCTION_H
