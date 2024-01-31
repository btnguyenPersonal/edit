#include <vector>
#include <string>
#include <climits>
#include "state.h"
#include "bounds.h"
#include "indent.h"
#include "helper.h"
#include "comment.h"

void toggleComment(State* state) {
    toggleCommentHelper(state, state->row, -1);
}

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
    int spaces = commentIndex != -1 ? commentIndex : getNumLeadingSpaces(line);
    state->data[row] = line.substr(0, spaces) + state->commentSymbol + ' ' + line.substr(spaces);
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
    unsigned int minIndentLevel = -1;
    if (foundNonComment) {
        minIndentLevel = UINT_MAX;
        for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
            unsigned int indent = getIndentLevel(state, i);
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
    if (isComment(state, state->data[state->row])) {
        for (int i = (int) state->row; i >= 0; i--) {
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
