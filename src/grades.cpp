#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "../Header Files/common.h"
#include "../Header Files/main.h"
#include "../Header Files/utilityfunctions.h"

using namespace std;

string pickSubject(const string& output);

vector<float> fillGrades() {
    vector<float> grades;

    string grade;
    do {
        cout << "Enter a grade and use . as a comma if it is a decimal number! If you're done, press the ENTER key and leave the line empty! " <<
        endl;
        getline(cin, grade);
        try {
            float gradeNr = stof(grade);
            grades.push_back(gradeNr);
        } catch(...) {
            if(!grade.empty())
                cout << "Enter a valid grade as a whole number or decimal number (use the period as a comma)! " << endl;
        }
    } while(!grade.empty());

    return grades;
}

void writeGrades(bool continueWriting) {
    string output = "Which subject gets new grades? Choose a number! ";
    cout << output << endl;

    auto subjects = readFile(false);

    for(int i = 0; i < subjects.size(); i++) {
        cout << i + 1 << ". " << subjects[i] << endl;
    }

    int chosenNumber = chooseNumber(output, subjects.size(), []() { continueProgram();});
    if(chosenNumber == -1)
        return;
    string subj = subjects[chosenNumber];

    fstream file(currentPath);

    string str;
    bool gotSubj = false;
    int position = 0;
    while(getline(file, str)) {
        if(gotSubj) {
            auto gradesList = fillGrades();
            string allGradesStr = str;
            for(float i : gradesList) {
                allGradesStr.append(to_string(i) + ",");
            }

            writeToFile(position, allGradesStr);
            break;
        }

        if(str == subj)
            gotSubj = true;

        position++;
    }

    file.close();
    if(continueWriting) {
        cout << "Do you want to add grades to another subject as well? If yes, input 'y'. " << endl;
        string decisionOtherSubject;
        getline(cin, decisionOtherSubject);
        if(decisionOtherSubject == "y" || decisionOtherSubject == "Y") {
            writeGrades(true);
            return;
        }
    }
    continueProgram();
}

void editGrades(bool remove) {
    string output = string("The grades of which subject do you want to ").append(remove ? "remove" : "edit").append("? Choose one of the following "
                                                                                                                    "numbers: ");
    string subj = pickSubject(output);
    if(subj.empty())
        return;

    string outputGrade = string("Which grade do you want to ").append(remove ? "remove?" : "edit?").append(" Pick a number or leave the line empty "
                                                                                                          "and press ENTER to cancel the action.");
    cout << outputGrade << endl;

    fstream file(currentPath);

    string str;
    bool foundSubject = false;
    int position = 0;
    while(getline(file, str)) {
        if(foundSubject) {
            if(str.empty()) {
                cout << "This subject doesn't contain any grades. Do you want to pick another subject [1] or give up [any other key]? " << endl;
                string decision;
                getline(cin, decision);
                if(decision == "1")
                    editGrades(remove);
                break;
            }

            stringstream grades(str);
            string segment;

            int i = 0;
            vector<string> gradesVector;
            while(getline(grades, segment, ',')) {
                i++;
                cout << i << ". " << segment << endl;
                gradesVector.push_back(segment);
            }

            int chosenNumber = chooseNumber(outputGrade, i, []() { continueProgram(); });
            if(chosenNumber == -1)
                return;

            if(remove)
                gradesVector.erase(gradesVector.begin() + chosenNumber);
            else {
                cout << "What do you want to overwrite this grade with? Please enter a whole number or a decimal number (use a dot as a 'comma'). If "
                        "you want to delete the grade, leave the line empty and press the ENTER key. " << endl;

                string newGrade;
                do {
                    cout << "Please enter a valid number and use the dot as a comma if necessary! " << endl;
                    getline(cin, newGrade);
                    if(newGrade.empty())
                        break;
                } while(!isFloat(newGrade));

                if(newGrade.empty())
                    gradesVector.erase(gradesVector.begin() + chosenNumber);
                else
                    gradesVector[chosenNumber] = newGrade;
            }

            string allGradesDone;
            for(const string& s : gradesVector) {
                allGradesDone.append(s + ",");
            }

            writeToFile(position, allGradesDone);
            break;
        }

        if(str == subj)
            foundSubject = true;
        position++;
    }
    file.close();
    continueProgram();
}