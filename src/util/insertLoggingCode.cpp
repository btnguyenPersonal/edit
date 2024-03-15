#include "insertLoggingCode.h"
#include "helper.h"
#include "indent.h"
#include <regex>
#include <string>
#include <vector>

void toggleLoggingCode(State* state, std::string variableName) {
    state->lastLoggingVar = variableName;
    std::string current = state->data[state->row];
    std::string loggingCode = getLoggingCode(state, state->row, variableName);
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

std::regex getRemoveLoggingRegex(State* state) {
    std::string extension = getExtension(state->filename);
    std::string pattern = "";
    if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
        pattern = "console\\.log\\('[0-9]+', .+?\\);";
    } else if (extension == "cpp") {
        pattern = "std::cout << \"[0-9]+ .+? << std::endl;";
    }
    std::regex logPattern(pattern);
    return logPattern;
}

void removeAllLoggingCode(State* state) {
    for (int i = state->data.size() - 1; i >= 0; i--) {
        std::regex logPattern = getRemoveLoggingRegex(state);
        if (std::regex_search(state->data[i], logPattern)) {
            state->data.erase(state->data.begin() + i);
        }
    }
}

std::string getLoggingCode(State* state, unsigned int row, std::string variableName) {
    std::string extension = getExtension(state->filename);
    std::string rowStr = std::to_string(row + 1);
    if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
        return "console.log('" + rowStr + "', " + "'" + variableName + "', " + variableName + ");";
    } else if (extension == "cpp") {
        return "std::cout << \"" + rowStr + " " + variableName + " \" << " + variableName + " << std::endl;";
    } else {
        return "";
    }
}
