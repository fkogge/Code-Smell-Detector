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

    struct LongMethod {
        SmellType type;
        size_t lineCount;
        string functionName;

        LongMethod(SmellType type, size_t lineCount, string functionName) {
            this->type = type;
            this->lineCount = lineCount;
            this->functionName = functionName;
        }
    };

    struct LongParameterList  {
        SmellType type;
        size_t parameterCount;
        string functionName;

        LongParameterList(SmellType type, size_t parameterCount, string functionName) {
            this->type = type;
            this->parameterCount = parameterCount;
            this->functionName = functionName;
        }
    };

    struct DuplicatedCode  {
        SmellType type;
        double similarityIndex;
        pair<string, string> functionNames;

        DuplicatedCode(SmellType type, double similarityIndex, string functionOne, string functionTwo) {
            this->type = type;
            this->similarityIndex = similarityIndex;
            this->functionNames = pair<string, string>(functionOne, functionTwo);
        }
    };

    CodeSmellDetector(const string &fileName, const vector<string> &linesFromFile);
    vector<string> getFunctionNames();
    vector<LongMethod> getLongMethodOccurrences() const;
    vector<LongParameterList> getLongParameterListOccurrences() const;
    vector<DuplicatedCode> getDuplicateCodeOccurrences() const;
    bool hasLongMethodSmell() const;
    bool hasLongParameterListSmell() const;
    bool hasDuplicateCodeSmell() const;
    bool hasSmell(SmellType smellType) const;
    static string smellTypeToString(SmellType type);

private:

    static const int MAX_LINES_OF_CODE = 15;
    static const int MAX_PARAMETER_COUNT = 3;
    static constexpr const double MAX_SIMILARITY_INDEX = 0.75;
    static const int POSSIBLE_CHARS = 255;
    static const char OPENING_CURLY_BRACKET;
    static const char CLOSING_CURLY_BRACKET;
    static const char OPENING_PAREN;

    vector<LongMethod> longMethodOccurences;
    vector<LongParameterList> longParameterListOccurences;
    vector<DuplicatedCode> duplicatedCodeOccurrences;

    size_t lineCount;
    vector<string> linesFromFile;
    string fileName;
    vector<Function> functionList;
    vector<string> functionNames;

    void skipBlankLines(size_t &currentLineNumber);
    void skipLinesUntilFunctionHeader(size_t &currentLineNumber);
    void skipLinesUntilOpeningCurlyBracket(size_t &currentLineNumber);
    size_t findFunctionClosingCurlyBracketLine(size_t startLineNumber);
    void extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber);
    void extractFunctions();
    void detectLongMethod();
    void detectLongParameterList();
    void detectDuplicatedCode();
    static bool containsCharacter(const string &str, const char &character);
    static pair<size_t, size_t> getSortedPair(size_t first, size_t second);
    double jaccardLineSimilarityIndex(vector<string> functionOneBody, vector<string> functionTwoBody);
    double jaccardTokenSimilarityIndex(vector<string> firstFunctionBody, vector<string> secondFunctionBody);
    double jaccardBiGramSimilarityIndex(string firstCodeString, string secondCodeString);
    double jaccardSimilarityIndex(string firstCodeString, string secondCodeString);
    static void computeFunctionTokenCounts(const vector<string> &functionBody, unordered_map<string, int> &tokenCounts);
    unordered_map<string, int> getAllUniqueTokenCounts(const unordered_map<string, int> &firstFunctionTokens, const unordered_map<string, int> &secondFunctionTokens);
};


#endif //CODESMELLDETECTOR_CODESMELLDETECTOR_H
