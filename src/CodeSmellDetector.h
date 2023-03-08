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

        LongMethod(SmellType type, size_t lineCount, const string &functionName) {
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
    explicit CodeSmellDetector(const vector<string> &linesFromFile);

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
    static bool isBlankLine(const string &line);
    static bool lineEndsWith(const string &line, const char &character);
    static bool isNotBeginningOfFunctionDefinition(const string &line);

    size_t findFunctionClosingCurlyBracketLine(size_t startLineNumber);

    // Store each line of code in functionContent (passed by reference)
    void extractFunctionContent(vector<string> &functionContent, size_t startLineNumber, size_t endLineNumber);
    void extractFunctions();

    // Code smell detection helper methods
    void detectLongMethod();
    void detectLongParameterList();
    void detectDuplicatedCode();

    /*
     * Calculates the Jaccard similarity indexes of two strings using bigram comparisons.
     * For example, if the input is two strings "abcd" and "abce", then the compared bigram sets are:
     *
     * - ["ab", "bc", "cd"]
     * - ["ab", "bc", "ce"]
     *
     * The intersection of the two sets is the set of matching bigram tokens across both sets:
     *
     * - ["ab", "bc"]
     *
     * The union of the two sets is all unique bigram tokens across either set:
     *
     * - ["ab", "bc", "cd", "ce"]
     *
     * Then the similarity index is calculated by dividing the intersection count by the union count:
     *
     * - 2 / 4 = 50%
     *
     * In the implementation, I just keep track of the counts instead of the actual sets since
     * creating the intersection and union sets is not necessary.
     */
    static double jaccardBiGramSimilarityIndex(const string &firstCodeString, const string &secondCodeString);
    static void fillBigramSet(unordered_set<string> &bigramSet, const string &codeString);
    static bool containsCharacter(const string &str, const char &character);
};


#endif //CODESMELLDETECTOR_CODESMELLDETECTOR_H
