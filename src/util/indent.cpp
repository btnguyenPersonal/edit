#include <vector>
#include <string>
#include "indent.h"
#include "helper.h"

int getNumLeadingSpaces(std::string s) {
    int numSpaces = 0;
    for (uint i = 0; i < s.length(); i++) {
        if (s[i] == ' ') {
            numSpaces++;
        } else {
            break;
        }
    }
    return numSpaces;
}

std::string getPrevLine(State* state) {
    for (int i = state->row - 1; i >= 0; i--) {
        if (state->data[i] != "") {
            return state->data[i];
        }
    }
    return "";
}

void indentLine(State* state) {
    // TODO add html
    ltrim(state->data[state->row]);

    std::string prevLine = getPrevLine(state);
    rtrim(prevLine);
    std::string currLine = state->data[state->row];
    int indentLevel = getNumLeadingSpaces(prevLine);

    for (uint i = 0; i < prevLine.length(); i++) {
        if (prevLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
            break;
        } else if (prevLine[i] == '(' || prevLine[i] == '{' || prevLine[i] == '[') {
            if (i + 1 == prevLine.length()) {
                indentLevel += state->indent;
            }
        }
    }

    for (uint i = 0; i < currLine.length(); i++) {
        if (currLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
            break;
        } else if (currLine[i] == ')' || currLine[i] == '}' || currLine[i] == ']') {
            if (i == 0) {
                indentLevel -= state->indent;
            }
        }
    }

    for (int i = 0; i < indentLevel; i++) {
        state->data[state->row] = ' ' + state->data[state->row];
    }
}
