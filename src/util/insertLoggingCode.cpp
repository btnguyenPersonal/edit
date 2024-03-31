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

std::string getRemoveLoggingPattern(State* state) {
    std::string extension = getExtension(state->filename);
    std::string pattern = "";
    if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
        pattern = "console\\.log\\('[0-9]+', .+?\\);";
    } else if (extension == "cpp") {
        pattern = "std::cout << \"[0-9]+ .+? << std::endl;";
    }
    return pattern;
}

void removeAllLoggingCode(State* state) {
    std::string logPattern = getRemoveLoggingPattern(state);
    if (logPattern != "") {
        for (int i = state->data.size() - 1; i >= 0; i--) {
            if (std::regex_search(state->data[i], std::regex(logPattern))) {
                state->data.erase(state->data.begin() + i);
            }
        }
    }
}

std::string getLoggingCode(State* state, unsigned int row, std::string variableName, bool showValue) {
    std::string extension = getExtension(state->filename);
    std::string rowStr = std::to_string(row + 1);
    if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
        std::string s = "console.log('" + rowStr + "', " + "'" + variableName + "'";
        if (showValue) {
            s += ", " + variableName;
        }
        s += ");";
        return s;
    } else if (extension == "cpp") {
        std::string s = "std::cout << \"" + rowStr + " " + variableName + "\"";
        if (showValue) {
            s += " << " + variableName;
        }
        s += " << std::endl;";
        return s;
    } else {
        return "";
    }
}
