#include "comment.h"
#include "bounds.h"
#include "helper.h"
#include "indent.h"
#include "state.h"
#include <climits>
#include <string>
#include <vector>

std::string trimLeadingComment(State* state, std::string line) {
    std::string outputLine = line;
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] != ' ') {
            if (line.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
                if (line.substr(i, state->commentSymbol.length() + 1) == state->commentSymbol + " ") {
                    outputLine = line.substr(0, i) + line.substr(i + 2);
                } else {
                    outputLine = line.substr(0, i) + line.substr(i + 1);
                }
                break;
            }
        }
    }
    return outputLine;
}

std::string trimComment(State* state, std::string line) {
    std::string outputLine = line;
    bool foundNonSpace = false;
    for (size_t i = 0; i < line.length(); i++) {
        if (line.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
            if (foundNonSpace == true) {
                outputLine = line.substr(0, i);
            }
            break;
        } else if (line[i] != ' ') {
            foundNonSpace = true;
        }
    }
    rtrim(outputLine);
    return outputLine;
}

void toggleComment(State* state) { toggleCommentHelper(state, state->row, -1); }

void toggleCommentHelper(State* state, unsigned int row, int commentIndex) {
    std::string line = state->data[row];
    if (commentIndex == -1) {
        int i = getNumLeadingSpaces(line);
        if (isCommentWithSpace(state, line)) {
            state->data[row] = line.substr(0, i) + line.substr(i + state->commentSymbol.length() + 1);
            return;
        } else if (isComment(state, line)) {
            state->data[row] = line.substr(0, i) + line.substr(i + state->commentSymbol.length());
            return;
        }
    }
    if (line.length() != 0) {
        int spaces = commentIndex != -1 ? commentIndex : getNumLeadingSpaces(line);
        state->data[row] = line.substr(0, spaces) + state->commentSymbol + ' ' + line.substr(spaces);
    }
}

bool isCommentWithSpace(State* state, std::string line) {
    ltrim(line);
    return line.substr(0, state->commentSymbol.length() + 1) == state->commentSymbol + ' ';
}

bool isComment(State* state, std::string line) {
    ltrim(line);
    return line.substr(0, state->commentSymbol.length()) == state->commentSymbol;
}

void toggleCommentLines(State* state, Bounds bounds) {
    bool foundNonComment = false;
    for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
        if (!isComment(state, state->data[i])) {
            foundNonComment = true;
            break;
        }
    }
    int minIndentLevel = -1;
    if (foundNonComment) {
        minIndentLevel = INT_MAX;
        for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
            int indent = getNumLeadingSpaces(state->data[i]);
            if (indent < minIndentLevel) {
                minIndentLevel = indent;
            }
        }
    }
    for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
        toggleCommentHelper(state, i, minIndentLevel);
    }
}

void unCommentBlock(State* state) {
    bool foundComment = false;
    Bounds bounds;
    bounds.minC = 0;
    bounds.maxC = 0;
    if (isComment(state, state->data[state->row])) {
        for (int i = (int)state->row; i >= 0; i--) {
            if (!isComment(state, state->data[i])) {
                state->row = i;
                break;
            }
        }
    }
    for (unsigned int i = state->row; i < state->data.size(); i++) {
        if (!foundComment) {
            if (isComment(state, state->data[i])) {
                foundComment = true;
                bounds.minR = i;
                bounds.maxR = i;
            }
        } else {
            if (!isComment(state, state->data[i])) {
                break;
            } else {
                bounds.maxR = i;
            }
        }
    }
    if (foundComment) {
        toggleCommentLines(state, bounds);
        state->row = bounds.minR;
    }
}
