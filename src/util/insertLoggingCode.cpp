#include <string>
#include <vector>
#include <regex>
#include "insertLoggingCode.h"
#include "indent.h"
#include "helper.h"

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

void removeAllLoggingCode(State* state) {
    for (int i = state->data.size() - 1; i >= 0; i--) {
        std::string pattern = "console\\.log\\('[0-9]+', .+?\\);";
        std::regex logPattern(pattern);
        if (std::regex_search(state->data[i], logPattern)) {
            state->data.erase(state->data.begin() + i);
        }
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
        return "console.log('" + rowStr + "', " + "'" + variableName + "', " + variableName + ");";
    } else if (extension == "cpp") {
        return "std::cout << " + rowStr + " << " + variableName + " << std::endl;";
    } else {
        return "";
    }
}
