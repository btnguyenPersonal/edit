#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "../util/indent.h"
#include "../util/comment.h"
#include "../util/bounds.h"
#include "../util/insertLoggingCode.h"
#include "sendVisualKeys.h"

void setStateFromWordPosition(State* state, WordPosition pos) {
    if (pos.min != 0 || pos.max != 0) {
        state->visual.col = pos.min;
        state->col = pos.max;
        state->visual.row = state->row;
    }
}

void surroundParagraph(State* state, bool includeLastLine) {
    auto start = state->row;
    for (int i = (int) start; i >= 0; i--) {
        if (state->data[i] == "") {
            break;
        } else {
            start = i;
        }
    }
    state->visual.row = start;
    auto end = state->row;
    for (unsigned int i = state->row; i < state->data.size(); i++) {
        if (state->data[i] == "") {
            if (includeLastLine) {
                end = i;
            }
            break;
        } else {
            end = i;
        }
    }
    state->row = end;
}

bool isValidMoveableChunk(State* state, Bounds bounds) {
    int start = getNumLeadingSpaces(state->data[bounds.minR]);
    for (unsigned int i = bounds.minR + 1; i <= bounds.maxR; i++) {
        if (getNumLeadingSpaces(state->data[i]) < start) {
            return false;
        }
    }
    return true;
}

std::string getInVisual(State* state) {
    Bounds bounds = getBounds(state);
    std::string clip = "";
    if (state->visualType == LINE) {
        for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
            clip += state->data[i] + "\n";
        }
    } else if (state->visualType == NORMAL) {
        unsigned int index = bounds.minC;
        for (size_t i = bounds.minR; i < bounds.maxR; i++) {
            while (index < state->data[i].size()) {
                clip += state->data[i][index];
                index += 1;
            }
            index = 0;
            clip += '\n';
        }
        clip += state->data[bounds.maxR].substr(index, bounds.maxC - index + 1);
    }
    return clip;
}

Bounds getBounds(State* state) {
    Bounds bounds;
    if (state->visual.col >= state->data[state->visual.row].length()) {
        if (state->data[state->visual.row].length() == 0) {
            state->visual.col = 0;
        } else {
            state->visual.col = state->data[state->visual.row].length() - 1;
        }
    }
    if (state->col >= state->data[state->row].length()) {
        if (state->data[state->row].length() == 0) {
            state->col = 0;
        } else {
            state->col = state->data[state->row].length() - 1;
        }
    }
    if (state->row < state->visual.row) {
        bounds.minR = state->row;
        bounds.minC = state->col;
        bounds.maxR = state->visual.row;
        bounds.maxC = state->visual.col;
    } else if (state->row > state->visual.row) {
        bounds.minR = state->visual.row;
        bounds.minC = state->visual.col;
        bounds.maxR = state->row;
        bounds.maxC = state->col;
    } else {
        bounds.minR = state->visual.row;
        bounds.maxR = state->row;
        bounds.minC = std::min(state->col, state->visual.col);
        bounds.maxC = std::max(state->col, state->visual.col);
    }
    return bounds;
}

Position changeInVisual(State* state) {
    Bounds bounds = getBounds(state);
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + bounds.minR, state->data.begin() + bounds.maxR);
        state->data[bounds.minR] = std::string("");
    } else if (state->visualType == NORMAL) {
        std::string firstPart = "";
        std::string secondPart = "";
        if (bounds.minC <= state->data[bounds.minR].length()) {
            firstPart = state->data[bounds.minR].substr(0, bounds.minC);
        }
        if (bounds.maxC < state->data[bounds.maxR].length()) {
            secondPart = state->data[bounds.maxR].substr(bounds.maxC + 1);
        }
        state->data[bounds.minR] = firstPart + secondPart;
        state->data.erase(state->data.begin() + bounds.minR + 1, state->data.begin() + bounds.maxR + 1);
    }
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    return pos;
}

Position copyInVisual(State* state) {
    Bounds bounds = getBounds(state);
    copyToClipboard(getInVisual(state));
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    return pos;
}

Position deleteInVisual(State* state) {
    Bounds bounds = getBounds(state);
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + bounds.minR, state->data.begin() + bounds.maxR + 1);
    } else if (state->visualType == NORMAL) {
        std::string firstPart = "";
        std::string secondPart = "";
        if (bounds.minC <= state->data[bounds.minR].length()) {
            firstPart = state->data[bounds.minR].substr(0, bounds.minC);
        }
        if (bounds.maxC < state->data[bounds.maxR].length()) {
            secondPart = state->data[bounds.maxR].substr(bounds.maxC + 1);
        }
        state->data[bounds.minR] = firstPart + secondPart;
        state->data.erase(state->data.begin() + bounds.minR + 1, state->data.begin() + bounds.maxR + 1);
    }
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    return pos;
}

void sendVisualKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
        state->mode = SHORTCUTS;
    } else if (state->prevKeys == "t") {
        state->col = toNextChar(state, c);
        state->prevKeys = "";
    } else if (state->prevKeys == "f") {
        state->col = findNextChar(state, c);
        state->prevKeys = "";
    } else if (state->prevKeys + c == "i`") {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "a`") {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "i\"") {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "a\"") {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "i'") {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "a'") {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "at") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "aT") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "ad") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "aB") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "ab") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "it") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "iT") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "id") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "iB") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "ib") {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "iw") {
        setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
        state->prevKeys = "";
    } else if (state->prevKeys + c == "ip") {
        state->visualType = LINE;
        surroundParagraph(state, false);
        state->prevKeys = "";
    } else if (state->prevKeys + c == "ap") {
        state->visualType = LINE;
        surroundParagraph(state, true);
        state->prevKeys = "";
    } else if (state->prevKeys + c == "gf") {
        std::vector<std::string> extensions = {"", ".js", ".jsx", ".ts", ".tsx"};
        for (unsigned int i = 0; i < extensions.size(); i++) {
            try {
                if (state->visualType == NORMAL) {
                    std::filesystem::path filePath(state->filename);
                    std::filesystem::path dir = filePath.parent_path();
                    auto newFilePath = dir / (getInVisual(state) + extensions[i]);
                    if (std::filesystem::exists(newFilePath.c_str())) {
                        auto baseDir = std::filesystem::current_path();
                        auto relativePath = std::filesystem::relative(newFilePath, baseDir);
                        state->resetState(relativePath.string());
                        break;
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
            }
        }
    } else if (state->prevKeys + c == "gg") {
        state->row = 0;
    } else if (c == 'g' || c == 'i' || c == 'a') {
        state->prevKeys += c;
    } else if (c == '^') {
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '0') {
        state->col = 0;
    } else if (c == '$') {
        if (state->data[state->row].length() != 0) {
            state->col = state->data[state->row].length() - 1;
        } else {
            state->col = 0;
        }
    } else if (c == 'f') {
        state->prevKeys = "f";
    } else if (c == 't') {
        state->prevKeys = "t";
    } else if (c == 'b') {
        state->col = b(state);
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
    } else if (c == 'w') {
        state->col = w(state);
    } else if (c == 'h') {
        left(state);
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == '{') {
        state->row = getPrevEmptyLine(state);
    } else if (c == '}') {
        state->row = getNextEmptyLine(state);
    } else if (c == '*') {
        state->searching = true;
        state->searchQuery = getInVisual(state);
        state->mode = SHORTCUTS;
        unsigned int temp_col = state->col;
        unsigned int temp_row = state->row;
        bool result = setSearchResult(state);
        if (result == false) {
            state->row = temp_row;
            state->col = temp_col - 1;
        }
    } else if (c == '#') {
        state->grepQuery = getInVisual(state);
        state->mode = GREP;
        generateGrepOutput(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'K') {
        Bounds bounds = getBounds(state);
        if (bounds.minR > 0) {
            if (isValidMoveableChunk(state, bounds)) {
                auto temp = state->data[bounds.minR - 1];
                state->data.erase(state->data.begin() + bounds.minR - 1);
                state->data.insert(state->data.begin() + bounds.maxR, temp);
                state->row = bounds.minR - 1;
                state->visual.row = bounds.maxR - 1;
                indentRange(state);
                state->col = getIndexFirstNonSpace(state);
            } else {
                state->status = "not a valid moveable chunk";
            }
        }
    } else if (c == 'J') {
        Bounds bounds = getBounds(state);
        if (bounds.maxR + 1 < state->data.size()) {
            if (isValidMoveableChunk(state, bounds)) {
                auto temp = state->data[bounds.maxR + 1];
                state->data.erase(state->data.begin() + bounds.maxR + 1);
                state->data.insert(state->data.begin() + bounds.minR, temp);
                state->row = bounds.minR + 1;
                state->visual.row = bounds.maxR + 1;
                indentRange(state);
                state->col = getIndexFirstNonSpace(state);
            } else {
                state->status = "not a valid moveable chunk";
            }
        }
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == 'V') {
        state->visualType = LINE;
    } else if (c == 'v') {
        state->visualType = NORMAL;
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else if (c == '=') {
        Bounds bounds = getBounds(state);
        state->row = bounds.minR;
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            indentLine(state);
            state->row += 1;
        }
        state->row = bounds.minR;
        state->visual.row = bounds.maxR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
    } else if (c == '<') {
        Bounds bounds = getBounds(state);
        state->row = bounds.minR;
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            deindent(state);
            state->row += 1;
        }
        state->row = bounds.minR;
        state->visual.row = bounds.maxR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
    } else if (c == '>') {
        Bounds bounds = getBounds(state);
        state->row = bounds.minR;
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            indent(state);
            state->row += 1;
        }
        state->row = bounds.minR;
        state->visual.row = bounds.maxR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
    } else if (c == 'p' || c == 'P') {
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        pasteFromClipboard(state);
        state->mode = SHORTCUTS;
    } else if (c == 'x') {
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
    } else if (c == 'd') {
        copyInVisual(state);
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
    } else if (c == 'y') {
        auto pos = copyInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
    } else if (c == ctrl('q')) {
        Bounds bounds = getBounds(state);
        for (state->row = bounds.minR; state->row <= bounds.maxR; state->row++) {
            insertLoggingCode(state);
        }
        state->row = bounds.minR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
    } else if (c == 'e') {
        Bounds bounds = getBounds(state);
        toggleCommentLines(state, bounds);
        state->row = bounds.minR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
    } else if (c == 's') {
        auto pos = changeInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = TYPING;
    } else if (c == 'c') {
        copyInVisual(state);
        auto pos = changeInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = TYPING;
    }
    if (!state->dontRecordKey) {
        state->motion += c;
    }
}
