#pragma once

bool isFloat(const std::string& str);
std::string trimLeft(const std::string& str, const char* delimiter);
std::string trimRight(const std::string& str, const char* delimiter);
[[maybe_unused]] std::string trim(const std::string& str, const char* delimiter);