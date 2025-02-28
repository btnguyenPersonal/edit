#include "insertLoggingCode.h"
#include "helper.h"
#include "indent.h"
#include <regex>
#include <string>
#include <vector>

void toggleLoggingCode(State* state, std::string variableName, bool showValue) {
    state->lastLoggingVar = variableName;
    std::string current = state->data[state->row];
    std::string loggingCode = getLoggingCode(state, state->row, variableName, showValue);
    if (loggingCode == "") {
        return;
    }
    if (state->row + 1 < state->data.size()) {
        auto nextLine = state->data[state->row + 1];
        ltrim(nextLine);
        if (nextLine == loggingCode) {
            state->data.erase(state->data.begin() + state->row + 1);
            return;
        }
    }
    state->data.insert(state->data.begin() + state->row + 1, loggingCode);
    indentLine(state, state->row + 1);
}

std::string getLoggingRegex(State* state) {
    std::string extension = getExtension(state->filename);
    std::string pattern = "";
    if (extension == "cpp") {
        pattern = "std::cout << \"[0-9]+\" .+? << std::endl;";
    } else {
        pattern = "console\\.log\\('[0-9]+', .+?\\);";
    }
    return pattern;
}

void removeAllLoggingCode(State* state) {
    std::string logPattern = getLoggingRegex(state);
    if (logPattern != "") {
        for (int i = state->data.size() - 1; i >= 0; i--) {
            if (std::regex_search(state->data[i], std::regex(logPattern))) {
                state->data.erase(state->data.begin() + i);
            }
        }
    }
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

std::string getLoggingCode(State* state, unsigned int row, std::string variableName, bool showValue) {
    std::string extension = getExtension(state->filename);
    std::string rowStr = std::to_string(row + 1);
    if (extension == "cpp") {
        std::string s = "std::cout << \"" + rowStr + " " + variableName + "\"";
        if (showValue) {
            s += " << " + variableName;
        }
        s += " << std::endl;";
        return s;
    } else {
        std::string s = "console.log('" + rowStr + "', " + "'" + replaceAll(variableName, "'", "\\'") + "'";
        if (showValue) {
            s += ", " + variableName;
        }
        s += ");";
        return s;
    }
}
