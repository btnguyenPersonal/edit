#include <string>
#include <vector>
#include "insertLoggingCode.h"
#include "helper.h"

void insertLoggingCode(State* state) {
    std::string current = state->data[state->row];
    std::string loggingCode = getLoggingCode(state, state->row);
    if (current.substr(0, loggingCode.length()) != loggingCode) {
        state->data[state->row] = loggingCode + current;
    }
}

void toggleLoggingCode(State* state) {
    std::string current = state->data[state->row];
    std::string loggingCode = getLoggingCode(state, state->row);
    if (current.substr(0, loggingCode.length()) == loggingCode) {
        state->data[state->row] = current.substr(loggingCode.length());
    } else {
        state->data[state->row] = loggingCode + current;
    }
}

void removeAllLoggingCode(State* state) {
    for (uint i = 0; i < state->data.size(); i++) {
        std::string current = state->data[i];
        std::string loggingCode = getLoggingCode(state, i);
        if (current.substr(0, loggingCode.length()) == loggingCode) {
            state->data[i] = current.substr(loggingCode.length());
        }
    }
}

std::string getLoggingCode(State* state, uint row) {
    std::string extension = getExtension(state->filename);
    std::string rowStr = std::to_string(row + 1);
    rowStr = std::string(4 - rowStr.length(), '0') + rowStr;
    if (extension == "js"
        || extension == "jsx"
        || extension == "ts"
        || extension == "tsx"
    ) {
        return "console.log('line', " + rowStr + ");";
    } else if (extension == "cpp") {
        return "std::cout << \"line\" << " + rowStr + " << std::endl;";
    } else {
        return "";
    }
}
