#pragma once
#include <vector>
#include "common.h"

void freshStart(char* pathname, bool notSoFresh = false);
void configurePath();
void takeYear(char* pathname, bool skipIntro = false, bool removeSubj = false);
void removeYear();
std::vector<std::string> readFile(bool evaluateGrades);
void continueProgram();
int chooseNumber(const std::string& output, int maxNumber, const FunctionExit& fnExit = nullptr);
void writeToFile(int position, const std::string& newContent, bool remove = false);