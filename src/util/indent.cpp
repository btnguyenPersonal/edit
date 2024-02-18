#include <vector>
#include <string>
#include "indent.h"
#include "helper.h"
#include "comment.h"

int getNumLeadingSpaces(std::string s) {
    int numSpaces = 0;
    for (unsigned int i = 0; i < s.length(); i++) {
        if (s[i] == ' ') {
            numSpaces++;
        } else {
            break;
        }
    }
    return numSpaces;
}

std::string getPrevLine(State* state, unsigned int row) {
    for (int i = row - 1; i >= 0; i--) {
        if (state->data[i] != "") {
            return state->data[i];
        }
    }
    return "";
}

int getIndentLevel(State* state, unsigned int row) {
    // TODO add html
    std::string prevLine = getPrevLine(state, row);
    prevLine = trimComment(state, prevLine);
    std::string currLine = state->data[row];
    ltrim(currLine);
    int indentLevel = getNumLeadingSpaces(prevLine);

    for (unsigned int i = 0; i < prevLine.length(); i++) {
        if (prevLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
            break;
        } else if (prevLine[i] == '(' || prevLine[i] == '{' || prevLine[i] == '[') {
            if (i + 1 == prevLine.length()) {
                indentLevel += state->indent;
            }
        }
    }

    for (unsigned int i = 0; i < currLine.length(); i++) {
        if (currLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
            break;
        } else if (currLine[i] == ')' || currLine[i] == '}' || currLine[i] == ']') {
            if (i == 0) {
                indentLevel -= state->indent;
            }
        }
    }

    return indentLevel;
}

void indentLine(State* state) {
    ltrim(state->data[state->row]);
    int indentLevel = getIndentLevel(state, state->row);
    for (int i = 0; i < indentLevel; i++) {
        state->data[state->row] = ' ' + state->data[state->row];
    }
}

void indentRange(State* state) {
    int indentDifference = getIndentLevel(state, state->row) - getNumLeadingSpaces(state->data[state->row]);
    if (indentDifference > 0) {
        for (int i = state->row; i <= state->visual.row; i++) {
            for (int j = 0; j < indentDifference; j++) {
                state->data[i] = ' ' + state->data[i];
            }
        }
    } else if (indentDifference < 0) {
        for (int i = state->row; i <= state->visual.row; i++) {
            state->data[i] = state->data[i].substr(-1 * indentDifference);
        }
    }
}
