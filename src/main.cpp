#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "CodeSmellDetector.h"

using namespace std;

class Hey {
public:
    Hey() {
        cout << "hey" << endl;
    }
    void hi() {
        cout << "hi" << endl;
        what();
    }


private:
    void what() {
        cout << "what" << endl;
    }
};

const int LONG_METHOD_OPTION = 1;
const int LONG_PARAMETER_LIST_OPTION = 2;
const int DUPLICATED_CODE_DETECTION_OPTION = 3;
const int QUIT_OPTION = 4;

void printIntro();
void fillFileNames(vector<string> &fileNames, int argCount, char *cmdLineArgs[]);
bool fillFileContents(vector<string> &fileContents, const string &filename);
bool invalidFileExtension(const string &filename);
void fillCodeSmellDetectorList(vector<CodeSmellDetector> &codeSmellDetectorList, const vector<vector<string>> &fileContents);
void displayMainMenu(CodeSmellDetector codeSmellDetector);
string selectMenuOption();
bool isValidOption(string userInput);

void printLongMethodInfo(const CodeSmellDetector &detector);
void printLongParameterListInfo(const CodeSmellDetector &codeSmellDetector);
void printDuplicatedCodeInfo(const CodeSmellDetector &codeSmellDetector);

int main(int argc, char *argv[]) {
    Hey hey;
    hey.hi();
    //const int QUIT = 4;
    printIntro();

    string filename = argv[1];
    if (invalidFileExtension(filename)) {
        cerr << "input file must have extension [.cpp]" << endl;
        return EXIT_FAILURE;
    }
    vector<string> fileContents;
    if(!fillFileContents(fileContents, filename)) {
        return EXIT_FAILURE;
    }

    vector<CodeSmellDetector> codeSmellDetectorList;
    //fillCodeSmellDetectorList(codeSmellDetectorList, fileContents);
    CodeSmellDetector codeSmellDetector(filename, fileContents);

    vector<string> functionNameList = codeSmellDetector.getFunctionNames();
    cout << "The file you provided contains the following methods: " << endl;
    for (const string &functionName : functionNameList) {
        cout << "\t-> " << functionName << endl;
    }

    int option = -1;
    string userInput;
    while (option != QUIT_OPTION) {
        do {
            displayMainMenu(codeSmellDetector);
            userInput = selectMenuOption();
        } while (!isValidOption(userInput));
        option = stoi(userInput);

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

bool fillFileContents(vector<string> &fileContents, const string& filename) {
    ifstream inputFile;
    inputFile.open(filename);

    string line;
    if (inputFile) {
        while (getline(inputFile, line)) {
            fileContents.push_back(line);
        }
    } else {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    inputFile.close();
    return true;
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

bool isValidOption(string userInput) {
    int option;
    try {
        option = stoi(userInput);
    } catch (const exception &e) {
        cout << "caught invalid input [" << userInput << "]: " << e.what() << endl;
        return false;
    }

    return option >= LONG_METHOD_OPTION && option <= QUIT_OPTION;
}


string selectMenuOption() {
    string option;
    cout << "> ";
    cin >> option;
    cin.ignore();
    cout << endl;
    return option;
}

bool invalidFileExtension(const string &filename) {
    size_t dotIndex = filename.find_last_of('.');
    return dotIndex == string::npos || filename.substr(dotIndex) != ".cpp";
}