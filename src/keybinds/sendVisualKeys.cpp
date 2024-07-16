#include "sendVisualKeys.h"
#include "../util/bounds.h"
#include "../util/clipboard.h"
#include "../util/comment.h"
#include "../util/helper.h"
#include "../util/indent.h"
#include "../util/insertLoggingCode.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/query.h"
#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

Bounds getBounds(State* state) {
    Bounds bounds;
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

void replaceAllWithChar(State* state, int c) {
    Bounds bounds = getBounds(state);
    if (state->visualType == NORMAL) {
        unsigned int col = bounds.minC;
        for (unsigned int row = bounds.minR; row < bounds.maxR; row++) {
            while (col < state->data[row].size()) {
                state->data[row][col] = c;
                col++;
            }
            col = 0;
        }
        while (col <= bounds.maxC && col < state->data[bounds.maxR].size()) {
            state->data[bounds.maxR][col] = c;
            col++;
        }
    } else if (state->visualType == BLOCK) {
        unsigned int min = std::min(bounds.minC, bounds.maxC);
        unsigned int max = std::max(bounds.minC, bounds.maxC);
        for (unsigned int row = bounds.minR; row <= bounds.maxR; row++) {
            for (unsigned int col = min; col <= max; col++) {
                if (col < state->data[row].size()) {
                    state->data[row][col] = c;
                }
            }
        }
    } else if (state->visualType == LINE) {
        for (unsigned int row = bounds.minR; row <= bounds.maxR; row++) {
            for (unsigned int col = 0; col < state->data[row].size(); col++) {
                state->data[row][col] = c;
            }
        }
    }
}

void changeCaseVisual(State* state, bool upper) {
    Bounds bounds = getBounds(state);
    if (state->visualType == NORMAL) {
        unsigned int col = bounds.minC;
        for (unsigned int row = bounds.minR; row < bounds.maxR; row++) {
            while (col < state->data[row].size()) {
                if (upper) {
                    state->data[row][col] = std::toupper(state->data[row][col]);
                } else {
                    state->data[row][col] = std::tolower(state->data[row][col]);
                }
                col++;
            }
            col = 0;
        }
        while (col <= bounds.maxC) {
            if (upper) {
                state->data[bounds.maxR][col] = std::toupper(state->data[bounds.maxR][col]);
            } else {
                state->data[bounds.maxR][col] = std::tolower(state->data[bounds.maxR][col]);
            }
            col++;
        }
    } else if (state->visualType == BLOCK) {
        unsigned int min = std::min(bounds.minC, bounds.maxC);
        unsigned int max = std::max(bounds.minC, bounds.maxC);
        for (unsigned int row = bounds.minR; row <= bounds.maxR; row++) {
            for (unsigned int col = min; col <= max; col++) {
                if (upper) {
                    state->data[row][col] = std::toupper(state->data[row][col]);
                } else {
                    state->data[row][col] = std::tolower(state->data[row][col]);
                }
            }
        }
    } else if (state->visualType == LINE) {
        for (unsigned int row = bounds.minR; row <= bounds.maxR; row++) {
            for (unsigned int col = 0; col < state->data[row].size(); col++) {
                if (upper) {
                    state->data[row][col] = std::toupper(state->data[row][col]);
                } else {
                    state->data[row][col] = std::tolower(state->data[row][col]);
                }
            }
        }
    }
}

void sortLines(State* state) {
    Bounds bounds = getBounds(state);
    std::vector<std::string> lines;
    for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
        lines.push_back(state->data[i]);
    }
    std::sort(lines.begin(), lines.end());
    int index = 0;
    for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
        state->data[i] = lines[index];
        index++;
    }
}

void setStateFromWordPosition(State* state, WordPosition pos) {
    if (pos.min != 0 || pos.max != 0) {
        state->visual.col = pos.min;
        state->col = pos.max;
        state->visual.row = state->row;
    }
}

void surroundParagraph(State* state, bool includeLastLine) {
    auto start = state->row;
    for (int i = (int)start; i >= 0; i--) {
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
        if (getNumLeadingSpaces(state->data[i]) < start && state->data[i] != "") {
            return false;
        }
    }
    return true;
}

bool visualBlockValid(State* state) {
    Bounds bounds = getBounds(state);
    for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
        if (bounds.maxC >= state->data[i].length()) {
            return false;
        }
    }
    return true;
}

std::string getInVisual(State* state) {
    Bounds bounds = getBounds(state);
    std::string clip = "";
    if (state->visualType == BLOCK) {
        unsigned int min = std::min(bounds.minC, bounds.maxC);
        unsigned int max = std::max(bounds.minC, bounds.maxC);
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            clip += safeSubstring(state->data[i], min, max + 1 - min) + "\n";
        }
    } else if (state->visualType == LINE) {
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
        clip += safeSubstring(state->data[bounds.maxR], index, bounds.maxC - index + 1);
    }
    return clip;
}

Position changeInVisual(State* state) {
    Bounds bounds = getBounds(state);
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + bounds.minR, state->data.begin() + bounds.maxR);
        state->data[bounds.minR] = std::string("");
    } else if (state->visualType == BLOCK) {
        deleteInVisual(state);
        unsigned int min = std::min(bounds.minC, bounds.maxC);
        pos.col = min;
    } else if (state->visualType == NORMAL) {
        std::string firstPart = "";
        std::string secondPart = "";
        if (bounds.minC <= state->data[bounds.minR].length()) {
            firstPart = safeSubstring(state->data[bounds.minR], 0, bounds.minC);
        }
        if (bounds.maxC < state->data[bounds.maxR].length()) {
            secondPart = safeSubstring(state->data[bounds.maxR], bounds.maxC + 1);
        }
        state->data[bounds.minR] = firstPart + secondPart;
        state->data.erase(state->data.begin() + bounds.minR + 1, state->data.begin() + bounds.maxR + 1);
    }
    return pos;
}

Position copyInVisual(State* state) {
    Bounds bounds = getBounds(state);
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    copyToClipboard(getInVisual(state));
    return pos;
}

Position deleteInVisual(State* state) {
    Bounds bounds = getBounds(state);
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + bounds.minR, state->data.begin() + bounds.maxR + 1);
    } else if (state->visualType == BLOCK) {
        unsigned int min = std::min(bounds.minC, bounds.maxC);
        unsigned int max = std::max(bounds.minC, bounds.maxC);
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string firstPart = safeSubstring(state->data[i], 0, min);
            std::string secondPart = safeSubstring(state->data[i], max + 1);
            state->data[i] = firstPart + secondPart;
        }
        pos.col = min;
    } else if (state->visualType == NORMAL) {
        std::string firstPart = "";
        std::string secondPart = "";
        if (bounds.minC <= state->data[bounds.minR].length()) {
            firstPart = safeSubstring(state->data[bounds.minR], 0, bounds.minC);
        }
        if (bounds.maxC < state->data[bounds.maxR].length()) {
            secondPart = safeSubstring(state->data[bounds.maxR], bounds.maxC + 1);
        }
        state->data[bounds.minR] = firstPart + secondPart;
        state->data.erase(state->data.begin() + bounds.minR + 1, state->data.begin() + bounds.maxR + 1);
    }
    return pos;
}

void logDotCommand(State* state) {
    state->dotCommand = state->motion;
    state->prevKeys = "";
    state->motion = "";
}

bool sendVisualKeys(State* state, char c, bool onlyMotions) {
    if (!state->dontRecordKey) {
        state->motion += c;
    }
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (state->prevKeys == "t") {
        state->col = toNextChar(state, c);
        state->prevKeys = "";
    } else if (state->prevKeys == "f") {
        state->col = findNextChar(state, c);
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == '`') {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == '`') {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == '"' ) {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == '"' ) {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == '\'') {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == '\'') {
        setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == 't') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == 'T') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == 'd') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == 'B') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == 'b') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 't') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 'T') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 'd') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 'B') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 'b') {
        setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 'w') {
        setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
        state->prevKeys = "";
    } else if (state->prevKeys == "i" && c == 'p') {
        state->visualType = LINE;
        surroundParagraph(state, false);
        state->prevKeys = "";
    } else if (state->prevKeys == "a" && c == 'p') {
        state->visualType = LINE;
        surroundParagraph(state, true);
        state->prevKeys = "";
    } else if (!onlyMotions && state->visualType == BLOCK && state->prevKeys == "g" && c == ctrl('s')) {
        Bounds bounds = getBounds(state);
        int iterations = 1;
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            getAndAddNumber(state, i, state->col, -1 * iterations);
            iterations++;
        }
        state->prevKeys = "";
    } else if (!onlyMotions && state->visualType == BLOCK && state->prevKeys == "g" && c == ctrl('a')) {
        Bounds bounds = getBounds(state);
        int iterations = 1;
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            getAndAddNumber(state, i, state->col, iterations);
            iterations++;
        }
        state->prevKeys = "";
    } else if (!onlyMotions && state->prevKeys == "g" && c == 'r') {
        if (state->visualType == NORMAL) {
            state->mode = SHORTCUTS;
            setQuery(&state->search, getInVisual(state));
            state->searching = true;
            searchFromTop(state);
        }
        state->prevKeys = "";
    } else if (!onlyMotions && state->prevKeys == "g" && c == 'f') {
        if (state->visualType == NORMAL) {
            std::vector<std::string> extensions = {"", ".js", ".jsx", ".ts", ".tsx"};
            for (unsigned int i = 0; i < extensions.size(); i++) {
                try {
                    std::filesystem::path filePath(state->filename);
                    std::filesystem::path dir = filePath.parent_path();
                    auto newFilePath = dir / (getInVisual(state) + extensions[i]);
                    if (std::filesystem::is_regular_file(newFilePath.c_str())) {
                        auto baseDir = std::filesystem::current_path();
                        auto relativePath = std::filesystem::relative(newFilePath, baseDir);
                        state->resetState(relativePath.string());
                        break;
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                }
            }
        }
        state->prevKeys = "";
    } else if (!onlyMotions && state->prevKeys == "g" && c == 'U') {
        changeCaseVisual(state, true);
        state->prevKeys = "";
        state->mode = SHORTCUTS;
    } else if (!onlyMotions && state->prevKeys == "r") {
        replaceAllWithChar(state, c);
        state->prevKeys = "";
        state->mode = SHORTCUTS;
    } else if (!onlyMotions && state->prevKeys == "g" && c == 'u') {
        changeCaseVisual(state, false);
        state->prevKeys = "";
        state->mode = SHORTCUTS;
    } else if (state->prevKeys == "g" && c == 'g') {
        state->row = 0;
        state->prevKeys = "";
    } else if (state->prevKeys != "") {
        state->prevKeys = "";
    } else if (c == 'g' || c == 'i' || c == 'a' || c == 'r') {
        state->prevKeys += c;
    } else if (!onlyMotions && state->visualType == BLOCK && c == ctrl('s')) {
        Bounds bounds = getBounds(state);
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            getAndAddNumber(state, i, state->col, -1);
        }
    } else if (!onlyMotions && state->visualType == BLOCK && c == ctrl('a')) {
        Bounds bounds = getBounds(state);
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            getAndAddNumber(state, i, state->col, 1);
        }
    } else if (!onlyMotions && c == 'm') {
        if (state->visualType == NORMAL) {
            toggleLoggingCode(state, getInVisual(state), true);
        }
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (!onlyMotions && c == ':') {
        if (state->visualType == NORMAL) {
            state->commandLine.query = "gs/" + getInVisual(state) + "/";
            state->commandLine.cursor = 3 + getInVisual(state).length() + 1;
            state->mode = COMMANDLINE;
        }
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
    } else if (!onlyMotions && c == '_') {
        sortLines(state);
        state->mode = SHORTCUTS;
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
    } else if (c == 'I' && state->visualType == BLOCK) {
        state->col = std::min(state->col, state->visual.col);
        state->mode = MULTICURSOR;
    } else if (c == 'A' && state->visualType == BLOCK) {
        state->col = std::max(state->col, state->visual.col) + 1;
        state->mode = MULTICURSOR;
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == '{') {
        state->row = getPrevEmptyLine(state);
    } else if (c == '}') {
        state->row = getNextEmptyLine(state);
    } else if (!onlyMotions && c == '*') {
        if (state->visualType == NORMAL) {
            state->searching = true;
            setQuery(&state->search, getInVisual(state));
            state->mode = SHORTCUTS;
            logDotCommand(state);
            setSearchResult(state);
        }
    } else if (!onlyMotions && c == '#') {
        setQuery(&state->grep, getInVisual(state));
        state->mode = GREP;
        generateGrepOutput(state);
    } else if (c == 'l') {
        right(state);
    } else if (!onlyMotions && c == ctrl('k')) {
        Bounds bounds = getBounds(state);
        for (unsigned int i = bounds.minR; i < bounds.maxR; i++) {
            state->col = state->data[bounds.minR].length();
            if (bounds.minR + 1 < state->data.size()) {
                ltrim(state->data[bounds.minR + 1]);
                state->data[bounds.minR] += " " + state->data[bounds.minR + 1];
                state->data.erase(state->data.begin() + bounds.minR + 1);
            }
        }
        state->row = bounds.minR;
        state->mode = SHORTCUTS;
    } else if (!onlyMotions && c == ctrl('j')) {
        Bounds bounds = getBounds(state);
        for (unsigned int i = bounds.minR; i < bounds.maxR; i++) {
            state->col = state->data[bounds.minR].length();
            if (bounds.minR + 1 < state->data.size()) {
                ltrim(state->data[bounds.minR + 1]);
                state->data[bounds.minR] += state->data[bounds.minR + 1];
                state->data.erase(state->data.begin() + bounds.minR + 1);
            }
        }
        state->row = bounds.minR;
        state->mode = SHORTCUTS;
    } else if (!onlyMotions && c == 'K') {
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
    } else if (!onlyMotions && c == 'J') {
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
    } else if (!onlyMotions && c == 'V') {
        state->visualType = LINE;
    } else if (!onlyMotions && c == 'v') {
        state->visualType = NORMAL;
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else if (!onlyMotions && c == '=') {
        Bounds bounds = getBounds(state);
        for (int i = bounds.minR; i <= (int)bounds.maxR; i++) {
            indentLine(state, i);
        }
        state->row = bounds.minR;
        state->visual.row = bounds.maxR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (c == '%') {
        auto pos = matchIt(state);
        state->row = pos.row;
        state->col = pos.col;
    } else if (!onlyMotions && c == '<') {
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
        logDotCommand(state);
    } else if (!onlyMotions && c == '>') {
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
        logDotCommand(state);
    } else if (!onlyMotions && (c == 'p' || c == 'P')) {
        pasteFromClipboardVisual(state);
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (!onlyMotions && c == 'x') {
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (!onlyMotions && c == 'd') {
        copyInVisual(state);
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (!onlyMotions && c == 'y') {
        auto pos = copyInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (!onlyMotions && c == 'e') {
        Bounds bounds = getBounds(state);
        toggleCommentLines(state, bounds);
        state->row = bounds.minR;
        state->col = getIndexFirstNonSpace(state);
        state->mode = SHORTCUTS;
        logDotCommand(state);
    } else if (!onlyMotions && c == 's') {
        auto pos = changeInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = TYPING;
    } else if (c == 'o') {
        auto tempRow = state->row;
        auto tempCol = state->col;
        state->row = state->visual.row;
        state->col = state->visual.col;
        state->visual.row = tempRow;
        state->visual.col = tempCol;
    } else if (!onlyMotions && c == 'c') {
        copyInVisual(state);
        auto pos = changeInVisual(state);
        if (state->visualType == BLOCK) {
            state->mode = MULTICURSOR;
        } else {
            state->mode = TYPING;
        }
        if (state->row != pos.row) {
            auto temp = state->row;
            state->row = pos.row;
            state->visual.row = temp;
        }
        state->col = pos.col;
    } else if (c == '9') {
        if (state->jumplist.index > 0) {
            state->jumplist.index--;
            state->row = state->jumplist.list[state->jumplist.index].row;
            state->col = state->jumplist.list[state->jumplist.index].col;
        }
    } else if (c == '8') {
        if (state->jumplist.index + 1 < state->jumplist.list.size()) {
            state->jumplist.index++;
            state->row = state->jumplist.list[state->jumplist.index].row;
            state->col = state->jumplist.list[state->jumplist.index].col;
        }
    } else {
        return false;
    }
    return true;
}
