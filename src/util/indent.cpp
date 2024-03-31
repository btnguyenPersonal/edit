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

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool hasHTML(std::string line, std::string extension) {
    if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx" || extension == "html") {
        auto trimmed = trim(line);
        if (trimmed.empty()) {
            return false;
        }
        return trimmed.front() == '<' || trimmed.back() == '>' || trimmed.front() == '>' || trimmed.back() == '<';
    }
    return false;
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

    if (hasHTML(prevLine, getExtension(state->filename))) {
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
                if (i > 0 && prevLine[i - 1] == '=') {
                    continue;
                } else if (tagType == EMPTY) {
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
    } else {
        for (unsigned int i = 0; i < prevLine.length(); i++) {
            if (prevLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
                break;
            } else if (prevLine[i] == '(' || prevLine[i] == '{' || prevLine[i] == '[') {
                if (i + 1 == prevLine.length()) {
                    indentLevel += state->indent;
                }
            }
        }
    }

    if (hasHTML(currLine, getExtension(state->filename))) {
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
                if (i > 0 && currLine[i - 1] == '=') {
                    continue;
                } else if (tagType == EMPTY) {
                    tagType = CLOSE;
                } else {
                    tagType = EMPTY;
                }
            }
        }
        if (tagType == CLOSE || tagStack < 0) {
            indentLevel -= state->indent;
        }
    } else {
        for (unsigned int i = 0; i < currLine.length(); i++) {
            if (currLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
                break;
            } else if (currLine[i] == ')' || currLine[i] == '}' || currLine[i] == ']') {
                if (i == 0) {
                    indentLevel -= state->indent;
                }
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
    unsigned int firstNonEmptyRow = state->row;
    for (int i = state->row; i <= (int)state->visual.row; i++) {
        if (state->data[i] != "") {
            firstNonEmptyRow = i;
            break;
        }
    }
    int indentDifference = getIndentLevel(state, state->row) - getNumLeadingSpaces(state->data[firstNonEmptyRow]);
    if (indentDifference > 0) {
        for (int i = state->row; i <= (int)state->visual.row; i++) {
            if (state->data[i] != "") {
                for (int j = 0; j < indentDifference; j++) {
                    state->data[i] = ' ' + state->data[i];
                }
            }
        }
    } else if (indentDifference < 0) {
        for (int i = state->row; i <= (int)state->visual.row; i++) {
            if (state->data[i] != "") {
                for (int j = 0; j < indentDifference; j++) {
                    state->data[i] = safeSubstring(state->data[i], 1);
                }
            }
        }
    }
}
