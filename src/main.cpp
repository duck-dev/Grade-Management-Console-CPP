#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <numeric>
#include "../Header Files/common.h"
#include "../Header Files/main.h"
#include "../Header Files/grades.h"
#include "../Header Files/subjects.h"
#include "../Header Files/utilityfunctions.h"

#pragma comment(lib, "shell32.lib")
using namespace std;

char forbiddenCharacters[] { '<', '>', ':', '"', '/', '\\', '|', '?', '*' }; // Array needs to be initialized

int main() {
    cout << endl << endl << "----------------------------------------------------------" << endl << "Welcome to your Grade Management system" <<
    endl << "----------------------------------------------------------" << endl << endl;

    configurePath();
}

void configurePath() {
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result != S_OK)
        cout << "Error: " << result << "\n";

    char* pathname = strcat(my_documents, "\\Grades Management");
    directory = pathname;

    struct stat info;

    if(stat(pathname, &info) != 0 )
        freshStart(pathname);
    else if( info.st_mode & S_IFDIR )
        takeYear(pathname);
    else
        printf( "%s is no directory\n", pathname );
}

void freshStart(char* pathnamePtr, bool notSoFresh) {
    string pathname(pathnamePtr);

    string output;
    if(notSoFresh)
        output = "Please enter the name of the school year or leave the line empty and press the ENTER key to stop this action: ";
    else
        output = "It seems you are new here or you have deleted your grades. Please enter the name of your school year: ";
    cout << output << endl;
    string yearName;
    getline(cin, yearName);

    for(char c : forbiddenCharacters) {
        if(yearName.find(c) != string::npos) {
            cout << "Please enter a valid name! The following characters are invalid in a file name:" << endl << forbiddenCharacters << endl;
            freshStart(pathnamePtr, notSoFresh);
            return;
        }
    }

    if(yearName.empty()) {
        takeYear(pathnamePtr);
        return;
    }

    if(notSoFresh) {
        ifstream file(pathname + "\\" + yearName + ".txt");
        if(file) {
            cout << "This file exists already. Do you want to overwrite this file? Enter 'y' if you want to overwrite it! " << endl;
            string overwrite;
            getline(cin, overwrite);
            if(overwrite != "y" && overwrite != "Y") {
                file.close();
                takeYear(pathnamePtr);
                return;
            }
        }
        file.close();
    } else {
        filesystem::create_directories(pathname);
    }
    string fileName = pathname + "\\" + yearName + ".txt";
    currentPath = fileName;

    inputSubjects();
}

void takeYear(char* pathname, bool skipIntro, bool removeSubj) {
    currentPath.clear();
    if(!skipIntro) {
        string yearDecisionBeginning;
        do {
            cout << "Do you want to inspect/edit an existing school year [1] or create a new one [2]? Choose 1 or 2! " << endl;
            getline(cin, yearDecisionBeginning);
            if(yearDecisionBeginning == "2") {
                freshStart(pathname, true);
                return;
            }
            else if(yearDecisionBeginning != "1") {
                cout << "Please enter either 1 or 2" << endl;
                continue;
            }
        } while(yearDecisionBeginning != "1" && yearDecisionBeginning != "2");
    }

    string output = string("Which school year would you like to ").append(removeSubj ? "remove?" : "inspect or edit?").append(" Choose a number! ");
    cout << output << endl;
    int i = 0;
    vector<string> pathNamesVector;
    for (const auto & entry : filesystem::directory_iterator(pathname)) {
        i++;
        string tempPath = entry.path().stem().string();
        cout << (to_string(i) + ".") + " " + tempPath << endl;
        pathNamesVector.push_back(tempPath);
    }

    if(i == 0) {
        cout << "There is no school year set up yet. Do you want to set up a new school year? If yes, type 'y' - otherwise the program will exit "
                "execution. " << endl;
        string decisionSetupSchoolYear;
        getline(cin, decisionSetupSchoolYear);
        if(decisionSetupSchoolYear == "y" || decisionSetupSchoolYear == "Y") {
            freshStart(pathname, true);
        }
        else {
            cout << "Exiting program... Goodbye, user! " << endl;
            exit(0);
        }
    }
    int chosenNumber = chooseNumber(output, i, [pathname]() { takeYear(pathname);});
    if(chosenNumber == -1)
        return;

    string pathStr(pathname);
    currentPath = pathStr + "\\" + pathNamesVector[chosenNumber] + ".txt";
    if(removeSubj) {
        vector<char> writable(currentPath.begin(), currentPath.end());
        writable.push_back('\0');
        if(remove(&writable[0]) != 0) {
            cout << "The file could not have been deleted." << endl;
        }
        takeYear(pathname);
    }
    else
        readFile(true);
}

vector<string> readFile(bool evaluateGrades) {
    ifstream file(currentPath);
    string str;
    string subject;
    int i = 0;
    vector<string> gradesStr;
    vector<string> subjectsVect;

    while (getline(file, str))
    {
        if(i == 0) {
            if(str.empty() && evaluateGrades)
                break;

            subject = str;
            if(!str.empty() && !isdigit(str.at(0)))
                subjectsVect.push_back(subject);
            if(evaluateGrades) {
                i++;
                continue;
            }
        }
        else if(i == 1 && evaluateGrades) {
            gradesStr.push_back(str);

            stringstream currentGrades(str);
            string segment;
            vector<float> gradesList;

            while(getline(currentGrades, segment, ',')) {
                if(isFloat(segment))
                    gradesList.push_back(stof(segment));
            }

            float average = 0;
            if(!gradesList.empty())
                average = std::accumulate(gradesList.begin(), gradesList.end(), 0.0f) / gradesList.size();

            cout << subject << ": " << average << endl;
            i = 0;
            continue;
        }
    }
    file.close();
    if(evaluateGrades)
        continueProgram();

    return subjectsVect;
}

void continueProgram() {
    cout << endl << "Press the ENTER button when you're ready to move on! " << endl;
    cin.get();
    cout << "What do you want to do? Choose one of the following numbers: " << endl;
    string possibilities[] { "Inspect subject", "Edit subject", "Add subject", "Remove subject", "Edit grades of a subject", "Add grades",
                             "Remove grades", "Select other school year", "Remove school year" };

    int max = sizeof(possibilities)/sizeof(*possibilities);
    for(int k = 0; k < max; k++) {
        cout << k + 1 << ". " << possibilities[k] << endl;
    }

    int chosenNumber = chooseNumber("What do you want to do? Choose one of the given numbers. ", max);
    switch(chosenNumber) { // Remember, chooseNumber() returns the chosen number - 1, so I can use it with zero-indexed collections
        case 0:
            inspectSubject();
            break;
        case 1:
            editSubject(false);
            break;
        case 2:
            addSubject();
            break;
        case 3:
            editSubject(true);
            break;
        case 4:
            editGrades(false);
            break;
        case 5:
            writeGrades(false);
            break;
        case 6:
            editGrades(true);
            break;
        case 7: {
            vector<char> writable(directory.begin(), directory.end());
            writable.push_back('\0');
            takeYear(&writable[0]);
            break;
        }
        case 8:
            removeYear();
            break;
        default:
            break;
    }
}

int chooseNumber(const string& output, int maxNumber, const FunctionExit& fnExit) {
    int numberChosen;
    string numberStr;
    bool firstTimeDone = false;

    do {
        if(firstTimeDone)
            cout << output << endl;

        firstTimeDone = true;
        getline(cin, numberStr);

        if(fnExit != nullptr && numberStr.empty()) {
            fnExit();
            return -1;
        }

        try {
            numberChosen = stoi(numberStr);
            if(numberChosen <= 0 || numberChosen > maxNumber)
                cout << "Please enter a valid number between 1 and " << maxNumber << "! " << endl;
            else
                return numberChosen - 1;
        } catch(...) {
            numberChosen = -1;
            cout << "Please enter a valid number between 1 and " << maxNumber << "! " << endl;
        }
    } while(numberChosen <= 0 || numberChosen > maxNumber);
    return 0;
}

void writeToFile(int position, const string& newContent, bool remove) {
    fstream file(currentPath);

    vector<string> lines;
    string str;
    while(getline(file, str)) {
        lines.push_back(str);
    }

    file.close();

    if(remove) {
        auto removePos = lines.begin() + position;
        lines.erase(removePos, removePos + 1);
    }
    else
        lines[position] = newContent;

    file.open(currentPath);
    for(const string& s : lines) {
        file << s << endl;
    }

    file.close();
}

void removeYear() {
    vector<char> writable(directory.begin(), directory.end());
    writable.push_back('\0');
    takeYear(&writable[0], true, true);
}
