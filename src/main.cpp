#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "CodeSmellDetector.h"
#include <csignal>
#include <algorithm>
#include <iomanip>

using namespace std;

const int LONG_METHOD_OPTION = 1;
const int LONG_PARAMETER_LIST_OPTION = 2;
const int DUPLICATED_CODE_DETECTION_OPTION = 3;
const int QUIT_OPTION = 4;

void printIntro();
bool fillFileContents(vector<string> &fileContents, const string &filename);
bool invalidFileExtension(const string &filename);
void run(const CodeSmellDetector &codeSmellDetector);
void displayMainMenu();
string selectMenuOption();
bool isValidOption(const string &userInput);

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

    try {
        CodeSmellDetector codeSmellDetector(fileContents);
        run(codeSmellDetector);
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return 0;
}

void printIntro() {
    cout << "Welcome to the Code Smell Detector program!" << endl;
    cout << "By Francis Kogge" << endl;
    cout << endl;
}

bool invalidFileExtension(const string &filename) {
    size_t dotIndex = filename.find_last_of('.');
    return dotIndex == string::npos || filename.substr(dotIndex) != ".cpp";
}

bool fillFileContents(vector<string> &fileContents, const string &filename) {
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

void run(const CodeSmellDetector &codeSmellDetector) {
    printFunctionNames(codeSmellDetector.getFunctionNames());

    int option;
    string userInput;

    do {
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
    } while (option != QUIT_OPTION);
}

void displayMainMenu() {
    cout << "\nPlease choose one of the following options: " << endl;
    cout << LONG_METHOD_OPTION << ". Long Method/Function Detection" << endl;
    cout << LONG_PARAMETER_LIST_OPTION << ". Long Parameter List Detection" << endl;
    cout << DUPLICATED_CODE_DETECTION_OPTION << ". Duplicated Code Detection" << endl;
    cout << QUIT_OPTION << ". Quit" << endl;
}

string selectMenuOption() {
    string option;
    cout << "> ";
    getline(cin, option);
    cout << endl;
    return option;
}

bool isValidOption(const string &userInput) {
    int option;
    string errorMessage = userInput + " is not a valid option. \nPlease choose an integer between " +
            to_string(LONG_METHOD_OPTION) + " and " + to_string(QUIT_OPTION);

    // Size check
    if (userInput.size() > 1) {
        cout << errorMessage << endl;
        return false;
    }

    // Decimal (float/double) check
    for (const char &c : userInput) {
        if (c == '.') {
            cout << errorMessage << endl;
            return false;
        }
    }

    // Convert to integer
    try {
        option = stoi(userInput);
    } catch (const exception &e) {
        cout << errorMessage << endl;
        return false;
    }

    // Range check
    bool isValid = (option >= LONG_METHOD_OPTION && option <= QUIT_OPTION);
    if (!isValid) {
        cout << errorMessage << endl;
    }

    return isValid;
}

void printFunctionNames(const vector<string> &functionNames) {
    cout << "The file you provided contains the following methods: " << endl;
    for (const string &name : functionNames) {
        cout << "\t-> " << name << endl;
    }
}

void printLongMethodInfo(const CodeSmellDetector &codeSmellDetector) {
    if (codeSmellDetector.hasLongMethodSmell()) {
        vector<CodeSmellDetector::LongMethod> longMethodOccurrences =
                codeSmellDetector.getLongMethodOccurrences();

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
    if (codeSmellDetector.hasLongParameterListSmell()) {
        vector<CodeSmellDetector::LongParameterList> longParameterListOccurrences =
                codeSmellDetector.getLongParameterListOccurrences();

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
    if (codeSmellDetector.hasDuplicateCodeSmell()) {
        vector<CodeSmellDetector::DuplicatedCode> duplicatedCodeOccurrences =
                codeSmellDetector.getDuplicateCodeOccurrences();

        for (const CodeSmellDetector::DuplicatedCode &occurrence : duplicatedCodeOccurrences) {
            cout << "The functions " << occurrence.functionNames.first << " and " << occurrence.functionNames.second
                 << " are duplicated. Their similarity percentage is "
                 << setprecision(2) << fixed << occurrence.similarityIndex * 100 << "%." // round 2 decimal places
                 //<< occurrence.similarityIndex
                 << endl;
        }
    } else {
        cout << "No functions contain Duplicated Code!" << endl;
    }
}