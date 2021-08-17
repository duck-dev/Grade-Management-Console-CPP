#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "../Header Files/common.h"
#include "../Header Files/main.h"
#include "../Header Files/grades.h"

using namespace std;

string pickSubject(const string& output) {
    cout << output << endl;

    auto subjects = readFile(false);

    for(int i = 0; i < subjects.size(); i++) {
        cout << i + 1 << ". " << subjects[i] << endl;
    }

    int chosenNumber = chooseNumber(output, subjects.size(), []() { continueProgram(); });
    if(chosenNumber == -1)
        return "";
    return subjects[chosenNumber];
}

void inputSubjects() {
    cout << "Enter all subjects you have and want to enter! When you are done, press the ENTER key on an empty line! " << endl;

    string subject;
    vector<string> subjects;

    ofstream outfile(currentPath);
    do {
        getline(cin, subject);
        if(find(subjects.begin(), subjects.end(), subject) == subjects.end()) { // Vector doesn't contain subject
            if(!subject.empty()) {
                subjects.push_back(subject);
                outfile << subject << endl << endl;
            }
        } else {
            cout << "You cannot specify a subject twice!" << endl;
        }
    } while(!subject.empty());

    outfile.close();

    cout << "The subjects have been entered!" << endl;
    cout << "Do you also want to enter grades? Enter 'y', if yes - otherwise, just press the Enter key! " << endl;

    string continueWithWritingGrades;
    getline(cin, continueWithWritingGrades);
    if(continueWithWritingGrades == "y" || continueWithWritingGrades == "Y") {
        writeGrades(true);
    } else {
        configurePath();
    }
}

void inspectSubject() {
    string subject = pickSubject("Which subject do you want to inspect?");
    ifstream infile(currentPath);

    string str;
    bool foundSubject = false;
    while(getline(infile, str)) {
        if(foundSubject) {
            cout << subject << endl;
            cout << str << endl; // str = grades
            break;
        }

        if(str == subject)
            foundSubject = true;
    }

    infile.close();
    continueProgram();
}

void editSubject(bool remove) {
    string output = string("Which subject do you want to ").append((remove ? "remove" : "edit")).append("? Pick a number! ");
    string subj = pickSubject(output);
    if(subj.empty())
        return;

    fstream file(currentPath);

    string str;
    int position = 0;
    while(getline(file, str)) {
        if(str == subj) {
            if(!remove) {
                cout << "Enter the new name of the current subject " << subj << " or leave the line empty and press ENTER to cancel the action. "
                << endl;
                string newName;
                getline(cin, newName);
                if(!newName.empty())
                    writeToFile(position, newName);
            } else {
                writeToFile(position, "", true);
            }

            file.close();
            continueProgram();
            return;
        }
        position++;
    }
}

void addSubject() {
    cout << "What is the name of the new subject you want to add? If you want to cancel this action, leave the line empty and press ENTER. "
    << endl;
    string newSubject;
    getline(cin, newSubject);
    if(newSubject.empty()) {
        continueProgram();
        return;
    }

    auto allSubjects = readFile(false);
    if(find(allSubjects.begin(), allSubjects.end(), newSubject) != allSubjects.end()) { // Vector contains subject
        cout << "This subject exists already! Please try it again with a new subject. " << endl;
        addSubject();
        return;
    }

    ofstream outfile(currentPath, ios_base::app);
    outfile << newSubject << endl;

    outfile.close();
    cout << "Do you also want to add grades to it? If yes, type 'y'. " << endl;
    string decision;
    getline(cin, decision);
    if(decision == "y" || decision == "Y") {
        auto grades = fillGrades();
        outfile.open(currentPath, ios_base::app);
        string allGradesStr;
        for(float i : grades) {
            allGradesStr.append(to_string(i) + ",");
        }

        outfile << allGradesStr << endl; // Maybe not endl?
        outfile.close();

        continueProgram();
        return;
    }

    outfile << endl;
    outfile.close();

    continueProgram();
}