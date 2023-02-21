//
// Created by Francis Kogge on 2/17/2023.
//

#ifndef CODESMELLDETECTOR_CODESMELLDETECTOR_H
#define CODESMELLDETECTOR_CODESMELLDETECTOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Function.h"

using namespace std;

class CodeSmellDetector {

public:

    enum SmellType {
        LONG_METHOD, LONG_PARAMETER_LIST, DUPLICATED_CODE
    };

    struct LongMethodOccurrence {
        SmellType type;
        int lineCount;
        string functionName;

        LongMethodOccurrence(SmellType type, int lineCount, string functionName) {
            this->type = type;
            this->lineCount = lineCount;
            this->functionName = functionName;
        }
    };

    struct LongParameterListOccurrence  {
        SmellType type;
        int parameterCount;
        string functionName;

        LongParameterListOccurrence(SmellType type, int parameterCount, string functionName) {
            this->type = type;
            this->parameterCount = parameterCount;
            this->functionName = functionName;
        }
    };

    struct DuplicateCodeOccurrence  {
        SmellType type;
        double similarityIndex;
        pair<string, string> functionNames;

        DuplicateCodeOccurrence(SmellType type, double similarityIndex, string functionOne, string functionTwo) {
            this->type = type;
            this->similarityIndex = similarityIndex;
           // pair<string, string> funcNames(functionOne, functionTwo);
            this->functionNames = pair<string, string>(functionOne, functionTwo);
        }
    };

    CodeSmellDetector(const string &fileName, const vector<string> &linesFromFile);
    vector<string> getFunctionNames();
    vector<LongMethodOccurrence> getLongMethodOccurrences() const;
    vector<LongParameterListOccurrence> getLongParameterListOccurrences() const;
    vector<DuplicateCodeOccurrence> getDuplicateCodeOccurrences() const;
    static string smellTypeToString(SmellType type);

private:

    static const int MAX_LINES_OF_CODE = 15;
    static const int MAX_PARAMETER_COUNT = 3;
    static constexpr const double MAX_SIMILARITY_INDEX = 0.75;
    static const int POSSIBLE_CHARS = 255;
    static const char OPENING_CURLY_BRACKET;
    static const char CLOSING_CURLY_BRACKET;
    static const char OPENING_PAREN;

    vector<LongMethodOccurrence> longMethodOccurences;
    vector<LongParameterListOccurrence> longParameterListOccurences;
    vector<DuplicateCodeOccurrence> duplicatedCodeOccurrences;

    int lineCount;
    vector<string> linesFromFile;
    string fileName;
    vector<Function> functionList;
    vector<string> functionNames;

    void extractFunctions();
    void detectLongMethod();
    void detectLongParameterList();
    void detectDuplicatedCode();
    bool containsCharacter(string str, const char &character);
    double calculateSimilarityIndex(string stringOne, string stringTwo);
    int findLineNumberWithClosingCurlyBracket(const string &startLine, int lineNumber);
};


#endif //CODESMELLDETECTOR_CODESMELLDETECTOR_H
