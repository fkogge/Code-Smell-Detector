//
// Created by Francis Kogge on 2/17/2023.
//

#ifndef CODESMELLDETECTOR_CODESMELLDETECTOR_H
#define CODESMELLDETECTOR_CODESMELLDETECTOR_H

#include <string>
#include <vector>
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


    /**
     * Initialize all fields and run code smell detection algorithms
     * @param linesFromFile lines of code from the input file
     */
    explicit CodeSmellDetector(const vector<string> &linesFromFile);

    /**
     * Get a list of function names extracted from the file
     * @return vector of function names
     */
    vector<string> getFunctionNames() const;

    /**
     * Get all occurrences of Long Method code smell
     * @return vector of LongMethod objects
     */
    vector<LongMethod> getLongMethodOccurrences() const;

    /**
     * Get all occurrences of Long Parameter List code smell
     * @return vector of LongParameterList objects
     */
    vector<LongParameterList> getLongParameterListOccurrences() const;

    /**
     * Get all occurrences of Duplicated Code smell
     * @return vector of DuplicatedCode objects
     */
    vector<DuplicatedCode> getDuplicateCodeOccurrences() const;

    /**
     * Was Long Method detected?
     * @return true if detected, false if not
     */
    bool hasLongMethodSmell() const;

    /**
    * Was Long Parameter List detected?
    * @return true if detected, false if not
    */
    bool hasLongParameterListSmell() const;

    /**
     * Was Duplicated Code detected?
     * @return true if detected, false if not
     */
    bool hasDuplicateCodeSmell() const;

    /**
     * Convert SmellType enum to string representation
     * @param type the enum
     * @return string representation
     */
    static string smellTypeToString(SmellType type);

private:
    // Code smell thresholds
    static const int MAX_LINES_OF_CODE = 15;
    static const int MAX_PARAMETER_COUNT = 3;
    static constexpr const double MAX_SIMILARITY_INDEX = 0.75;
    static const string INCLUDE_DIRECTIVE;

    // Lists to store code smell occurrences
    vector<LongMethod> longMethodOccurrences;
    vector<LongParameterList> longParameterListOccurrences;
    vector<DuplicatedCode> duplicatedCodeOccurrences;

    // Lists to store processed functions
    vector<Function> functionList;
    vector<string> functionNames;

    // Store each line of code in functionContent (passed by reference)
    void extractFunctions(const vector<string> &linesFromFile);

    // Code smell detection helper methods
    void detectLongMethod();
    void detectLongParameterList();
    void detectDuplicatedCode();

    /*
     * Calculates the Jaccard similarity indexes of two strings using character set comparisons.
     * For example, if the input is two strings "abcd" and "abce", then the compared sets are:
     *
     * - ['a', 'b', 'c', 'd']
     * - ['a', 'b', 'c', 'e']
     *
     * The intersection of the two sets is the set of matching characters across both sets:
     *
     * - ['a', 'b', 'c']
     *
     * The union of the two sets is all unique characters across either set:
     *
     * - ['a', 'b', 'c', 'd', 'e']
     *
     * Then the similarity index is calculated by dividing the intersection count by the union count:
     *
     * - 3 / 5 = 60%
     *
     * In the implementation, I just keep track of the counts instead of the actual sets since
     * creating the intersection and union sets is not necessary to calculate the similarity index.
     */
    static double jaccardSimilarityIndex(const string &firstCodeString, const string &secondCodeString);

    // Helper for filling the character set
    static void fillCharSet(unordered_set<char> &charSet, const string &codeString);
};


#endif //CODESMELLDETECTOR_CODESMELLDETECTOR_H
