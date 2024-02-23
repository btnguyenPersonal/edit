#include <string>
#include <vector>
#include <regex>
#include "insertLoggingCode.h"
#include "helper.h"

void insertLoggingCode(State* state) {
    std::string current = state->data[state->row];
    std::string loggingCode = getLoggingCode(state, state->row, "");
    if (current.substr(0, loggingCode.length()) != loggingCode) {
        state->data[state->row] = loggingCode + current;
    }
}

void toggleLoggingCode(State* state, std::string variableName) {
    std::string current = state->data[state->row];
    std::string loggingCode = getLoggingCode(state, state->row, variableName);
    if (current.substr(0, loggingCode.length()) == loggingCode) {
        state->data[state->row] = current.substr(loggingCode.length());
        if (state->col > loggingCode.length()) {
            state->col -= loggingCode.length();
        } else {
            state->col = 0;
        }
    } else {
        state->data[state->row] = loggingCode + current;
        state->col += loggingCode.length();
    }
}

void removeAllLoggingCode(State* state) {
    for (unsigned int i = 0; i < state->data.size(); i++) {
        std::string pattern = "console\\.log\\('line', '" + std::to_string(i + 1) + "', .+?\\);";
        std::regex logPattern(pattern);
        state->data[i] = std::regex_replace(state->data[i], logPattern, "");
    }
}

std::string getLoggingCode(State* state, unsigned int row, std::string variableName) {
    std::string extension = getExtension(state->filename);
    std::string rowStr = std::to_string(row + 1);
    if (extension == "js"
        || extension == "jsx"
        || extension == "ts"
        || extension == "tsx"
    ) {
        return "console.log('line', '" + rowStr + "', " + variableName + ");";
    } else if (extension == "cpp") {
        return "std::cout << \"line\" << " + rowStr + " << " + variableName + " << std::endl;";
    } else {
        return "";
    }
}
