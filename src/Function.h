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
    Function(vector<string> codeLines, int startLineNumber, int endDefLineNumber, int endLineNumber);
    int getNumberOfLinesOfCode() const;
    int getNumberOfParameters() const;
    string getName() const;
    string getCodeString() const;

private:
    vector<string> codeLines;
    string name;
    int numLinesOfCode;
    int numParameters;
    string codeString;
    string extractName();
    string transformToCodeString();
    int extractParameterCount();
    string getFunctionHeader() const;

};


#endif //CODESMELLDETECTOR_FUNCTION_H
