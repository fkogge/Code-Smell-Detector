#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "CodeSmellDetector.h"
#include <csignal>
#include <algorithm>

using namespace std;

const int LONG_METHOD_OPTION = 1;
const int LONG_PARAMETER_LIST_OPTION = 2;
const int DUPLICATED_CODE_DETECTION_OPTION = 3;
const int QUIT_OPTION = 4;

void printIntro();
bool fillFileContents(vector<string> &fileContents, const string &filename);
bool invalidFileExtension(const string &filename);
void displayMainMenu();
string selectMenuOption();
bool isValidOption(string userInput);
void run(const CodeSmellDetector &codeSmellDetector);

void printFunctionNames(const vector<string> &functionNames);
void printLongMethodInfo(const CodeSmellDetector &codeSmellDetector);
void printLongParameterListInfo(const CodeSmellDetector &codeSmellDetector);
void printDuplicatedCodeInfo(const CodeSmellDetector &codeSmellDetector);

int main(int argc, char *argv[]) {
    // Handle error when resizing terminal window
    // Ok to just ignore signal
    signal(SIGWINCH, SIG_IGN);

    printIntro();

    if (argc != 2) {
        cerr << "usage: " << argv[0] << " " << "FILENAME" << endl;
        return EXIT_FAILURE;
    }

    string filename = argv[1];
    if (invalidFileExtension(filename)) {
        cerr << "input file must have extension [.cpp]" << endl;
        return EXIT_FAILURE;
    }

    vector<string> fileContents;
    if(!fillFileContents(fileContents, filename)) {
        cerr << "error opening file: [" << filename << "]" << endl;
        return EXIT_FAILURE;
    }

    CodeSmellDetector codeSmellDetector(fileContents);

    run(codeSmellDetector);

    return 0;
}

void printFunctionNames(const vector<string> &functionNames) {
    cout << "The file you provided contains the following methods: " << endl;
    for (const string &name : functionNames) {
        cout << "\t-> " << name << endl;
    }
}

void printLongMethodInfo(const CodeSmellDetector &codeSmellDetector) {
    vector<CodeSmellDetector::LongMethod> longMethodOccurrences = codeSmellDetector.getLongMethodOccurrences();

    if (codeSmellDetector.hasLongMethodSmell()) {
        for (const CodeSmellDetector::LongMethod &longMethod : longMethodOccurrences) {
            cout << "The " << longMethod.functionName
                 << " function is a " << CodeSmellDetector::smellTypeToString(longMethod.type)
                 << ". It contains " << longMethod.lineCount << " lines of code. "
                 << endl;
        }
    } else {
        cout << "No function has Long Method!" << endl;
    }
}

void printLongParameterListInfo(const CodeSmellDetector &codeSmellDetector) {
    vector<CodeSmellDetector::LongParameterList> longParameterListOccurrences =
            codeSmellDetector.getLongParameterListOccurrences();

    if (codeSmellDetector.hasLongParameterListSmell()) {
        for (const CodeSmellDetector::LongParameterList &occurrence : longParameterListOccurrences) {
            cout << "The " << occurrence.functionName
                 << " function has a " << CodeSmellDetector::smellTypeToString(occurrence.type)
                 << ". It contains " << occurrence.parameterCount << " parameters. "
                 << endl;
        }
    } else {
        cout << "No function has Long Parameter List!" << endl;
    }
}

void printDuplicatedCodeInfo(const CodeSmellDetector &codeSmellDetector) {
    vector<CodeSmellDetector::DuplicatedCode> duplicatedCodeOccurrences =
            codeSmellDetector.getDuplicateCodeOccurrences();

    if (codeSmellDetector.hasDuplicateCodeSmell()) {
        for (const CodeSmellDetector::DuplicatedCode &occurrence : duplicatedCodeOccurrences) {
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

bool fillFileContents(vector<string> &fileContents, const string& filename) {
    ifstream inputFile;
    inputFile.open(filename);

    string line;
    if (inputFile) {
        while (getline(inputFile, line)) {
            fileContents.push_back(line);
        }
    } else {
        return false;
    }

    inputFile.close();
    return true;
}

void displayMainMenu() {
    cout << "\nPlease choose one of the following options: " << endl;
    cout << LONG_METHOD_OPTION << ". Long Method/Function Detection" << endl;
    cout << LONG_PARAMETER_LIST_OPTION << ". Long Parameter List Detection" << endl;
    cout << DUPLICATED_CODE_DETECTION_OPTION << ". Duplicated Code Detection" << endl;
    cout << QUIT_OPTION << ". Quit" << endl;
}

void run(const CodeSmellDetector &codeSmellDetector) {
    printFunctionNames(codeSmellDetector.getFunctionNames());

    int option = -1;
    string userInput;

    while (option != QUIT_OPTION) {
        do {
            displayMainMenu();
            userInput = selectMenuOption();
        } while (!isValidOption(userInput));

        option = stoi(userInput);

        if (option == LONG_METHOD_OPTION) {
            printLongMethodInfo(codeSmellDetector);
        } else if (option == LONG_PARAMETER_LIST_OPTION) {
            printLongParameterListInfo(codeSmellDetector);
        } else if (option == DUPLICATED_CODE_DETECTION_OPTION) {
            printDuplicatedCodeInfo(codeSmellDetector);
        }
    }
}

bool isValidOption(string userInput) {
    int option;
    string errorMessage = userInput + " is not a valid option. \nPlease choose an integer between "
                          + to_string(LONG_METHOD_OPTION) + " and " + to_string(QUIT_OPTION);

    if (userInput.size() > 1) {
        cout << errorMessage << endl;
        return false;
    }

    for (const char &c : userInput) {
        if (c == '.') {
            cout << errorMessage << endl;
            return false;
        }
    }

    try {
        option = stoi(userInput);
    } catch (const exception &e) {
        cout << errorMessage << endl;
        return false;
    }

    bool isValid = option >= LONG_METHOD_OPTION && option <= QUIT_OPTION;
    if (!isValid) {
        cout << errorMessage << endl;
    }
    return isValid;
}

string selectMenuOption() {
    string option;
    cout << "> ";
    getline(cin, option);
    cout << endl;
    return option;
}

bool invalidFileExtension(const string &filename) {
    size_t dotIndex = filename.find_last_of('.');
    return dotIndex == string::npos || filename.substr(dotIndex) != ".cpp";
}