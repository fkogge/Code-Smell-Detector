#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "CodeSmellDetector.h"

using namespace std;

const int LONG_METHOD_OPTION = 1;
const int LONG_PARAMETER_LIST_OPTION = 2;
const int DUPLICATED_CODE_DETECTION_OPTION = 3;
const int QUIT_OPTION = 4;

void printIntro();
void fillFileNames(vector<string> &fileNames, int argCount, char *cmdLineArgs[]);
void fillFileContents(vector<string> &fileContents, const string &filename);
void fillCodeSmellDetectorList(vector<CodeSmellDetector> &codeSmellDetectorList, const vector<vector<string>> &fileContents);
void displayMainMenu(CodeSmellDetector codeSmellDetector);
int selectMenuOption();

void printLongMethodInfo(const CodeSmellDetector &detector);
void printLongParameterListInfo(const CodeSmellDetector &codeSmellDetector);
void printDuplicatedCodeInfo(const CodeSmellDetector &codeSmellDetector);

int main(int argc, char *argv[]) {
    //const int QUIT = 4;
    printIntro();

    string filename = argv[1];
   // vector<string> fileNames;
   // fillFileNames(fileNames, argc, argv);

    vector<string> fileContents;
    fillFileContents(fileContents, filename);

//    for (size_t i = 0; i < fileContents.size(); i++) {
//        for (size_t j = 0; j < fileContents[i].size(); j++) {
//            cout << fileContents[i][j] << endl;
//        }
//
//    }

    vector<CodeSmellDetector> codeSmellDetectorList;
    //fillCodeSmellDetectorList(codeSmellDetectorList, fileContents);
    CodeSmellDetector codeSmellDetector(filename, fileContents);

    vector<string> functionNameList = codeSmellDetector.getFunctionNames();
    cout << "The file you provided contains the following methods: " << endl;
    for (const string &functionName : functionNameList) {
        cout << "\t-> " << functionName << endl;
    }

    int option = -1;
    while (option != QUIT_OPTION) {
        displayMainMenu(codeSmellDetector);
        option = selectMenuOption();

        if (option == LONG_METHOD_OPTION) {
            printLongMethodInfo(codeSmellDetector);
        } else if (option == LONG_PARAMETER_LIST_OPTION) {
            printLongParameterListInfo(codeSmellDetector);
        } else if (option == DUPLICATED_CODE_DETECTION_OPTION) {
            printDuplicatedCodeInfo(codeSmellDetector);
        } else if (option != QUIT_OPTION) {
            cout << "Invalid option. Please try again.";
        }
    }


    return 0;
}

void printLongMethodInfo(const CodeSmellDetector &detector) {
    vector<CodeSmellDetector::LongMethodOccurrence> longMethodOccurences = detector.getLongMethodOccurrences();
    bool hasLongMethodSmell = !longMethodOccurences.empty();

    if (hasLongMethodSmell) {
        for (CodeSmellDetector::LongMethodOccurrence occurence : longMethodOccurences) {
            cout << "The " << occurence.functionName
                 << " function is a " << CodeSmellDetector::smellTypeToString(occurence.type)
                 << ". It contains " << occurence.lineCount << " lines of code. "
                 << endl;
        }
    } else {
        cout << "No function has Long Method!" << endl;
    }
}

void printLongParameterListInfo(const CodeSmellDetector &codeSmellDetector) {
    vector<CodeSmellDetector::LongParameterListOccurrence> longParameterListOccurrences =
            codeSmellDetector.getLongParameterListOccurrences();
    bool hasLongParameterListSmell = !longParameterListOccurrences.empty();

    if (hasLongParameterListSmell) {
        for (CodeSmellDetector::LongParameterListOccurrence occurence : longParameterListOccurrences) {
            cout << "The " << occurence.functionName
                 << " function has a " << CodeSmellDetector::smellTypeToString(occurence.type)
                 << ". It contains " << occurence.parameterCount << " parameters. "
                 << endl;
        }
    } else {
        cout << "No function has Long Parameter List!" << endl;
    }
}

void printDuplicatedCodeInfo(const CodeSmellDetector &codeSmellDetector) {
    vector<CodeSmellDetector::DuplicateCodeOccurrence> duplicatedCodeOccurrences =
            codeSmellDetector.getDuplicateCodeOccurrences();
    bool hasDuplicatedCodeSmell = !duplicatedCodeOccurrences.empty();

    if (hasDuplicatedCodeSmell) {
        for (CodeSmellDetector::DuplicateCodeOccurrence occurrence : duplicatedCodeOccurrences) {
            cout << "The functions " << occurrence.functionNames.first << " and " << occurrence.functionNames.second
                 << " are duplicated. Their similarity percentage is " << occurrence.similarityIndex * 100
                 << endl;
        }
    } else {
        cout << "No functions contain Duplicated Code!" << endl;
    }
}

void printIntro() {
    cout << "Welcome to the Code Smell Detector program!" << endl;
    cout << "By Francis Kogge" << endl;
    cout << endl;
}

void fillFileNames(vector<string> &fileNames, int argCount, char *cmdLineArgs[]) {
    for (int i = 1; i < argCount; i++) {
        fileNames.push_back(cmdLineArgs[i]);
    }
}

void fillFileContents(vector<string> &fileContents, const string& filename) {

    //vector<string> linesFromFile;
    ifstream inputFile;
    inputFile.open(filename);

    string line;
    if (inputFile) {
        while (getline(inputFile, line)) {
            fileContents.push_back(line);
        }
    } else {
        cerr << "Error opening file: " << filename << endl;
    }

    inputFile.close();
}

void fillCodeSmellDetectorList(vector<CodeSmellDetector> &codeSmellDetectorList, const vector<vector<string>> &fileContents) {
    for (const auto &linesFromFile : fileContents) {
        CodeSmellDetector codeSmellDetector("x", linesFromFile);
        codeSmellDetectorList.push_back(codeSmellDetector);
    }
}

void displayMainMenu(CodeSmellDetector codeSmellDetector) {


    cout << "\nPlease choose one of the following options: " << endl;
    cout << LONG_METHOD_OPTION << ". Long Method/Function Detection" << endl;
    cout << LONG_PARAMETER_LIST_OPTION << ". Long Parameter List Detection" << endl;
    cout << DUPLICATED_CODE_DETECTION_OPTION << ". Duplicated Code Detection" << endl;
    cout << QUIT_OPTION << ". Quit" << endl;
}

int selectMenuOption() {
    int option;
    cout << "> ";
    cin >> option;
    cin.ignore();
    cout << endl;
    return option;
}