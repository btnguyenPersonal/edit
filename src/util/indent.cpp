#include "indent.h"
#include "comment.h"
#include "helper.h"
#include <string>
#include <vector>

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

bool hasHTML(std::string line) {
    if (line.empty()) return false;
    return line.front() == '<' || line.back() == '>' || line.front() == '>' || line.back() == '<';
}

enum TagType {
    EMPTY,
    OPEN,
    CLOSE,
};

int getIndentLevel(State* state, unsigned int row) {
    std::string prevLine = getPrevLine(state, row);
    prevLine = trimComment(state, prevLine);
    std::string currLine = state->data[row];
    ltrim(currLine);
    int indentLevel = getNumLeadingSpaces(prevLine);

    if (hasHTML(prevLine)) {
        int tagType = EMPTY;
        int tagStack = 0;
        for (unsigned int i = 0; i < prevLine.length(); i++) {
            if (tagType == EMPTY && prevLine[i] == '<') {
                if (i + 1 < prevLine.length() && prevLine[i + 1] == '/') {
                    tagType = CLOSE;
                    tagStack--;
                } else {
                    tagType = OPEN;
                    tagStack++;
                }
            } else if (prevLine[i] == '>') {
                if (tagType == EMPTY) {
                    tagType = CLOSE;
                } else {
                    if (i - 1 >= 0 && prevLine[i - 1] == '/') {
                        tagStack--;
                    }
                    tagType = EMPTY;
                }
            }
        }
        if (tagType == OPEN || tagStack > 0) {
            indentLevel += state->indent;
        }
    }

    if (hasHTML(currLine)) {
        int tagType = EMPTY;
        int tagStack = 0;
        for (unsigned int i = 0; i < currLine.length(); i++) {
            if (tagType == EMPTY && currLine[i] == '<') {
                if (i + 1 < currLine.length() && currLine[i + 1] == '/') {
                    tagType = CLOSE;
                    tagStack--;
                } else {
                    tagType = OPEN;
                    tagStack++;
                }
            } else if (currLine[i] == '>') {
                if (tagType == EMPTY) {
                    tagType = CLOSE;
                } else {
                    tagType = EMPTY;
                }
            }
        }
        if (tagType == CLOSE || tagStack < 0) {
            indentLevel -= state->indent;
        }
    }

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

void indentLine(State* state, unsigned int row) {
    ltrim(state->data[row]);
    if (state->data[row].length() != 0) {
        int indentLevel = getIndentLevel(state, row);
        for (int i = 0; i < indentLevel; i++) {
            state->data[row] = ' ' + state->data[row];
        }
    }
}

void indentLine(State* state) {
    ltrim(state->data[state->row]);
    if (state->data[state->row].length() != 0) {
        int indentLevel = getIndentLevel(state, state->row);
        for (int i = 0; i < indentLevel; i++) {
            state->data[state->row] = ' ' + state->data[state->row];
        }
    }
}

void indentRange(State* state) {
    int indentDifference = getIndentLevel(state, state->row) - getNumLeadingSpaces(state->data[state->row]);
    if (indentDifference > 0) {
        for (int i = state->row; i <= (int)state->visual.row; i++) {
            for (int j = 0; j < indentDifference; j++) {
                state->data[i] = ' ' + state->data[i];
            }
        }
    } else if (indentDifference < 0) {
        for (int i = state->row; i <= (int)state->visual.row; i++) {
            state->data[i] = state->data[i].substr(-1 * indentDifference);
        }
    }
}
