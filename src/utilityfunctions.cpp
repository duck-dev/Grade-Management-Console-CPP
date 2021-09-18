#include <sstream>

bool isFloat(const std::string& str) {
    std::istringstream iss(str);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

std::string trimLeft(const std::string &str, const char* delimiter) {
    size_t start = str.find_first_not_of(delimiter);
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string trimRight(const std::string &str, const char* delimiter) {
    size_t end = str.find_last_not_of(delimiter);
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

[[maybe_unused]] std::string trim(const std::string &str, const char* delimiter) {
    return trimRight(trimLeft(str, delimiter), delimiter);
}
