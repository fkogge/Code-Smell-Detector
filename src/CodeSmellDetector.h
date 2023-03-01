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

/**
 * Detects three types of code smells: Long Method, Long Parameter List, and Duplicated Code.
 * Takes a list of lines of code from the file as input.
 */
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
        int parameterCount;
        string functionName;

        LongParameterList(SmellType type, int parameterCount, const string &functionName) {
            this->type = type;
            this->parameterCount = parameterCount;
            this->functionName = functionName;
        }
    };

    struct DuplicatedCode  {
        SmellType type;
        double similarityIndex;
        pair<string, string> functionNames;

        DuplicatedCode(SmellType type, double similarityIndex, const string &functionOne, const string &functionTwo) {
            this->type = type;
            this->similarityIndex = similarityIndex;
            this->functionNames = pair<string, string>(functionOne, functionTwo);
        }
    };

    /*
     * Initialize all fields and run code smell detection algorithms
     */
    CodeSmellDetector(const vector<string> &linesFromFile);

    /*
     * Get a list of function names extracted from the file
     */
    vector<string> getFunctionNames() const;

    /*
     * Get all occurrences of Long Method code smell
     */
    vector<LongMethod> getLongMethodOccurrences() const;

    /*
     * Get all occurrences of Long Parameter List code smell
     */
    vector<LongParameterList> getLongParameterListOccurrences() const;

    /*
     * Get all occurrences of Duplicated Code smell
     */
    vector<DuplicatedCode> getDuplicateCodeOccurrences() const;

    /*
     * Was Long Method detected?
     */
    bool hasLongMethodSmell() const;

    /*
     * Was Long Parameter List detected?
     */
    bool hasLongParameterListSmell() const;

    /*
     * Was Duplicate Code detected?
     */
    bool hasDuplicateCodeSmell() const;

    /*
     * Convert SmellType enum to string representation
     */
    static string smellTypeToString(SmellType type);

private:
    // Code smell thresholds
    static const int MAX_LINES_OF_CODE = 15;
    static const int MAX_PARAMETER_COUNT = 3;
    static constexpr const double MAX_SIMILARITY_INDEX = 0.75;

    vector<LongMethod> longMethodOccurrences;
    vector<LongParameterList> longParameterListOccurrences;
    vector<DuplicatedCode> duplicatedCodeOccurrences;

    size_t fileLineCount;
    vector<string> linesFromFile;
    vector<Function> functionList;
    vector<string> functionNames;

    // Skip lines while updating currentLineNumber (passed by reference)
    void skipBlankLines(size_t &currentLineNumber);
    void skipLinesUntilFunctionHeader(size_t &currentLineNumber);
    void skipLinesUntilOpeningCurlyBracket(size_t &currentLineNumber);

    size_t findFunctionClosingCurlyBracketLine(size_t startLineNumber);

    // Store each line of code in functionContent (passed by reference)
    void extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber);
    void extractFunctions();

    // Code smell detection helper methods
    void detectLongMethod();
    void detectLongParameterList();
    void detectDuplicatedCode();

    /*
     * Calculates the Jaccard similarity index of the two function bodies. This does a token by token comparison
     * of the counts of each token, to consider that certain tokens could be repeated (i.e. common keywords such as
     * for, while, int). Given that Jaccard similarity is (intersection) / (union of two sets), I define the
     * intersection as the counts of each unique matching token seen in both functions, while I define the union as
     * the sum of the counts of all unique tokens seen across either function. This will provide a more accurate
     * calculation of whether the two functions are duplicated.
     */
    static double jaccardTokenSimilarityIndex(const vector<string> &firstFunctionBody, const vector<string> &secondFunctionBody);
    static void computeFunctionTokenCounts(const vector<string> &functionBody, unordered_map<string, int> &tokenCounts);
    static pair<size_t, size_t> getSortedPair(size_t first, size_t second);
    static unordered_map<string, int> getAllUniqueTokenCounts(const unordered_map<string, int> &firstFunctionTokens, const unordered_map<string, int> &secondFunctionTokens);

    static bool containsCharacter(const string &str, const char &character);
};


#endif //CODESMELLDETECTOR_CODESMELLDETECTOR_H
