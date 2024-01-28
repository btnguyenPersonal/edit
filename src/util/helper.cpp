#include "../global.h"
#include <algorithm>
#include <string>
#include <iterator>
#include <vector>
#include <climits>
#include <fstream>
#include <ncurses.h>
#include <iostream>
#include "state.h"
#include "helper.h"
#include "visualType.h"

void moveHarpoonRight(State* state) {
    if (state->harpoonIndex + 1 < state->harpoonFiles.size()) {
        std::string temp = state->harpoonFiles[state->harpoonIndex];
        state->harpoonFiles[state->harpoonIndex] = state->harpoonFiles[state->harpoonIndex + 1];
        state->harpoonFiles[state->harpoonIndex + 1] = temp;
        state->harpoonIndex += 1;
    }
}

void moveHarpoonLeft(State* state) {
    if (state->harpoonIndex > 0) {
        std::string temp = state->harpoonFiles[state->harpoonIndex];
        state->harpoonFiles[state->harpoonIndex] = state->harpoonFiles[state->harpoonIndex - 1];
        state->harpoonFiles[state->harpoonIndex - 1] = temp;
        state->harpoonIndex -= 1;
    }
}

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

void replaceAll(State* state, std::string query, std::string replace) {
    for (uint i = 0; i < state->data.size(); i++) {
        if (query.empty()) {
            return;
        }
        size_t startPos = 0;
        while ((startPos = state->data[i].find(query, startPos)) != std::string::npos) {
            state->data[i].replace(startPos, query.length(), replace);
            startPos += replace.length();
        }
    }
}

bool setSearchResultReverse(State* state) {
    fixColOverMax(state);
    uint initialCol = state->col;
    uint initialRow = state->row;
    uint col = initialCol;
    uint row = initialRow;
    bool isFirst = true;
    do {
        std::string line = isFirst ? state->data[row].substr(0, col) : state->data[row];
        size_t index = line.rfind(state->searchQuery);
        if (index != std::string::npos) {
            state->row = row;
            state->col = static_cast<unsigned int>(index);
            return true;
        }
        if (row == 0) {
            row = state->data.size() - 1;
        } else {
            row--;
        }
        isFirst = false;
    } while (row != initialRow);
    // try last row again
    std::string line = state->data[row];
    size_t index = line.rfind(state->searchQuery);
    if (index != std::string::npos) {
        state->row = row;
        state->col = static_cast<unsigned int>(index);
        return true;
    }
    return false;
}

// TODO make this look cleaner
bool setSearchResult(State* state) {
    fixColOverMax(state);
    uint initialCol = state->col;
    uint initialRow = state->row;
    uint col = initialCol;
    uint row = initialRow;
    do {
        std::string line = state->data[row].substr(col);
        size_t index = line.find(state->searchQuery);
        if (index != std::string::npos) {
            state->row = row;
            state->col = static_cast<unsigned int>(index) + col;
            return true;
        }
        row = (row + 1) % state->data.size();
        col = 0;
    } while (row != initialRow);
    // try last row again
    std::string line = state->data[row];
    size_t index = line.find(state->searchQuery);
    if (index != std::string::npos) {
        state->row = row;
        state->col = static_cast<unsigned int>(index) + col;
        return true;
    }
    return false;
}

void setPosition(State* state, Position pos) {
    state->row = pos.row;
    state->col = pos.col;
}

void initVisual(State* state, VisualType visualType) {
    state->visualType = visualType;
    state->visual.row = state->row;
    state->visual.col = state->col;
}

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

char ctrl(char c) {
    return c - 'a' + 1;
}

bool isAlphaNumeric(char c) {
    // TODO replace all instances w/ std call
    return std::isalnum(c);
}

uint getIndent(const std::string& str) {
    for (uint i = 0; i < str.length(); i++) {
        if (str[i] != ' ') {
            return i;
        }
    }
    return 0;
}

uint getPrevLineSameIndent(State* state) {
    uint current = getIndent(state->data[state->row]);
    for (int i = (int) state->row - 1; i >= 0; i--) {
        if (current == getIndent(state->data[i])) {
            return i;
        }
    }
    return state->row;
}

uint getNextLineSameIndent(State* state) {
    uint current = getIndent(state->data[state->row]);
    for (uint i = state->row + 1; i < state->data.size(); i++) {
        if (current == getIndent(state->data[i])) {
            return i;
        }
    }
    return state->row;
}

WordPosition findQuoteBounds(const std::string &str, char quoteChar, uint cursor, bool includeQuote) {
    int lastQuoteIndex = -1;
    for (uint i = 0; i <= cursor; i++) {
        if (str[i] == quoteChar) {
            lastQuoteIndex = i;
        }
    }
    uint i;
    for (i = cursor + 1; i < str.length(); i++) {
        if (str[i] == quoteChar) {
            if (lastQuoteIndex != -1 && lastQuoteIndex < (int) cursor) {
                break;
            } else {
                if (lastQuoteIndex == -1) {
                    lastQuoteIndex = i;
                } else {
                    break;
                }
            }
        }
    }
    if (i != str.length()) {
        if (i - lastQuoteIndex == 1 || includeQuote) {
            return {(uint) lastQuoteIndex, (uint) i};
        } else {
            return {(uint) lastQuoteIndex + 1, (uint) i - 1};
        }
    } else {
        return {0, 0};
    }
}


WordPosition findParentheses(const std::string &str, char openParen, char closeParen, uint cursor, bool includeParen) {
    int balance = 0;
    int openParenIndex = -1;
    // look back for openParen
    for (int i = cursor; i >= 0; i--) {
        if (str[i] == openParen) {
            if (balance == 0) {
                openParenIndex = i;
                break;
            } else {
                balance--;
            }
        } else if (str[i] == closeParen) {
            balance++;
        }
    }
    balance = 0;
    // if haven't found yet look forward for openParen
    if (openParenIndex == -1) {
        for (int i = cursor; i < (int) str.length(); i++) {
            if (str[i] == openParen) {
                if (balance == 0) {
                    openParenIndex = i;
                    break;
                } else {
                    balance--;
                }
            } else if (str[i] == closeParen) {
                balance++;
            }
        }
    }
    balance = 0;
    // if haven't found return {0,0}
    if (openParenIndex == -1) {
        return {0,0};
    }
    for (int i = openParenIndex + 1; i < (int) str.length(); i++) {
        if (str[i] == openParen) {
            balance--;
        } else if (str[i] == closeParen) {
            if (balance == 0) {
                if (i - openParenIndex == 1 || includeParen) {
                    return {(uint) openParenIndex, (uint) i};
                } else {
                    return {(uint) openParenIndex + 1, (uint) i - 1};
                }
            } else {
                balance++;
            }
        }
    }
    return {0, 0};
}

WordPosition getWordPosition(const std::string& str, uint cursor) {
    if (cursor >= str.size()) {
        return {0, 0};
    }
    // Move cursor to the start of the current chunk
    while (cursor > 0 && str[cursor - 1] != ' ' && str[cursor] != ' ' && (std::isalnum(str[cursor]) == std::isalnum(str[cursor - 1]))) {
        cursor--;
    }
    // If cursor is on a space, move to the next chunk
    if (str[cursor] == ' ') {
        while (cursor < str.size() && str[cursor] == ' ') {
            cursor++;
        }
    }
    // If no non-space chunk is found
    if (cursor >= str.size()) {
        return {0, 0};
    }
    // Find the end of the chunk
    uint start = cursor;
    uint end = start;
    while (end < str.size() && str[end] != ' ' && (std::isalnum(str[start]) == std::isalnum(str[end]))) {
        end++;
    }

    return {start, end - 1};
}

bool filePathContainsSubstring(const std::filesystem::path& filePath, const std::string& query) {
    // TODO make fzf
    std::string filePathStr = filePath.string();
    std::string queryLower = query;

    // Convert both strings to lower case for case-insensitive comparison
    std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    return filePathStr.find(queryLower) != std::string::npos;
}

bool shouldIgnoreFile(const std::filesystem::path& path) {
    std::vector<std::string> ignoreList = {".git", "node_modules", "build", "dist"};
    for (uint i = 0; i < ignoreList.size(); i++) {
        if (path.string().find(ignoreList[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<grepMatch> grepFiles(const std::filesystem::path& dir_path, const std::string& query) {
    std::vector<grepMatch> matches;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (shouldIgnoreFile(entry.path())) {
            continue;
        }
        if (std::filesystem::is_regular_file(entry)) {
            std::ifstream file(entry.path());
            std::string line;
            int lineNumber = 0;
            while (std::getline(file, line)) {
                lineNumber++;
                if (line.find(query) != std::string::npos) {
                    grepMatch match = grepMatch();
                    match.path = std::filesystem::relative(entry.path(), dir_path);
                    match.lineNum = lineNumber;
                    match.line = line;
                    matches.push_back(match);
                }
            }
        }
    }
    return matches;
}

std::vector<std::filesystem::path> findFiles(const std::filesystem::path& dir_path, const std::string& query) {
    std::vector<std::filesystem::path> matching_files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (shouldIgnoreFile(entry.path())) {
            continue;
        }
        if (std::filesystem::is_regular_file(entry) && filePathContainsSubstring(entry.path(), query)) {
            matching_files.push_back(std::filesystem::relative(entry.path(), dir_path));
        }
    }
    std::sort(matching_files.begin(), matching_files.end());
    return matching_files;
}

void generateGrepOutput(State* state) {
    if (state->grepQuery == "") {
        state->grepOutput.clear();
    } else {
        state->grepOutput = grepFiles(std::filesystem::current_path(), state->grepQuery);
    }
}

void generateFindFileOutput(State* state) {
    state->findFileOutput = findFiles(std::filesystem::current_path(), state->findFileQuery);
}

uint w(State* state) {
    bool isSpecial = !isAlphaNumeric(state->data[state->row][state->col]);
    bool isOnSpace = state->data[state->row][state->col] == ' ';
    bool space = false;
    for (uint i = state->col + 1; i < state->data[state->row].size(); i += 1) {
        if (state->data[state->row][i] == ' ') {
            space = true;
        } else if (isOnSpace || isSpecial == isAlphaNumeric(state->data[state->row][i])) {
            return i;
        } else if ((isOnSpace || space) && !isSpecial == isAlphaNumeric(state->data[state->row][i])) {
            return i;
        }
    }
    return state->col;
}

uint b(State* state) {
    bool isSpecial = !isAlphaNumeric(state->data[state->row][state->col]);
    bool isOnSpace = state->data[state->row][state->col] == ' ';
    bool space = false;
    uint ret = state->col;
    for (uint i = state->col; i > 0; i -= 1) {
        if (state->data[state->row][i - 1] == ' ') {
            space = true;
        } else if (isOnSpace || isSpecial == isAlphaNumeric(state->data[state->row][i - 1])) {
            ret = i - 1;
            break;
        } else if ((isOnSpace || space) && !isSpecial == isAlphaNumeric(state->data[state->row][i - 1])) {
            ret = i;
            break;
        }
    }
    bool currentAlpha = isAlphaNumeric(state->data[state->row][ret]);
    for (uint i = ret; i > 0; i -= 1) {
        if (state->data[state->row][i - 1] == ' ') {
            break;
        } else if (currentAlpha == isAlphaNumeric(state->data[state->row][i - 1])) {
            ret = i - 1;
        } else {
            break;
        }
    }
    return ret;
}

void saveFile(std::string filename, std::vector<std::string> data) {
    std::ofstream file(filename);
    if (!data.empty()) {
        for (size_t i = 0; i < data.size() - 1; ++i) {
            file << data[i] << "\n";
        }
        file << data.back();
    }
}

std::vector<std::string> readFile(std::string filename) {
    std::ifstream file(filename);
    std::string str;
    std::vector<std::string> file_contents;
    while (file.good()) {
        std::getline(file, str);
        file_contents.push_back(str);
    }
    return file_contents;
}

bool isWindowPositionInvalid(State* state) {
    if (state->row < state->windowPosition.row || (int) state->row - (int) state->windowPosition.row > ((int) state->maxY - 2)) {
        return true;
    } else if (state->col < state->windowPosition.col || (int) state->col - (int) state->windowPosition.col > ((int) state->maxX - LINE_NUM_OFFSET - 1)) {
        return true;
    }
    return false;
}

void centerScreen(State* state) {
    if (state->row < state->maxY / 2) {
        state->windowPosition.row = 0;
    } else {
        state->windowPosition.row = state->row - state->maxY / 2;
    }
    if (state->col < state->windowPosition.col) {
        state->windowPosition.col = state->col;
    } else if (state->col > state->windowPosition.col + (state->maxX - LINE_NUM_OFFSET - 1)) {
        state->windowPosition.col = state->col + LINE_NUM_OFFSET + 1 - state->maxX;
    }
}

void upHalfScreen(State* state) {
    for (uint i = 0; i < state->maxY / 2; i++) {
        if (state->row > 0) {
            state->row -= 1;
            state->windowPosition.row -= 1;
        }
    }
}

void downHalfScreen(State* state) {
    for (uint i = 0; i < state->maxY / 2; i++) {
        if (state->row < state->data.size() - 1) {
            state->row += 1;
            state->windowPosition.row += 1;
        }
    }
}

void up(State* state) {
    if (state->row > 0) {
        state->row -= 1;
    }
    if (state->row < state->windowPosition.row) {
        state->windowPosition.row -= 1;
    }
}

void down(State* state) {
    if (state->row < state->data.size() - 1) {
        state->row += 1;
    }
    if ((int) state->row - (int) state->windowPosition.row > ((int) state->maxY - 2)) {
        state->windowPosition.row += 1;
    }
}

void left(State* state) {
    if (state->col >= state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
    if (state->col > 0) {
        state->col -= 1;
    }
}

void right(State* state) {
    if (state->col >= state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
    if (state->col < state->data[state->row].length()) {
        state->col += 1;
    }
}

void indent(State* state) {
    for (uint i = 0; i < state->indent; i++) {
        state->data[state->row] = " " + state->data[state->row];
    }
}

void deindent(State* state) {
    for (uint i = 0; i < state->indent; i++) {
        if (state->data[state->row].substr(0, 1) == " ") {
            state->data[state->row] = state->data[state->row].substr(1);
        }
    }
}

int getIndexFirstNonSpace(State* state) {
    int i;
    for (i = 0; i < (int) state->data[state->row].length(); i++) {
        if (state->data[state->row][i] != ' ') {
            return i;
        }
    }
    return i;
}

void calcWindowBounds() {
    int y, x;
    getmaxyx(stdscr, y, x);
    State::setMaxYX(y, x);
}

void insertEmptyLineBelow(State* state) {
    state->data.insert(state->data.begin() + state->row + 1, "");
}

void insertEmptyLine(State* state) {
    state->data.insert(state->data.begin() + state->row, "");
}

bool handleMotion(State* state, char c, std::string motion) {
    size_t i;
    for (i = 0; i < state->prevKeys.length(); i++) {
        if (state->prevKeys[i] != motion[i]) {
            return false;
        }
    }
    if (motion[i] == c) {
        if (i < motion.length() - 1) {
            state->prevKeys += c;
        } else {
            state->motion = state->prevKeys + c;
            state->motionComplete = true;
        }
        return true;
    } else {
        return false;
    }
}

int maximum(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int minimum(int a, int b) {
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

void sanityCheckDocumentEmpty(State* state) {
    if (state->data.size() == 0) {
        state->data.push_back("");
    }
}

void fixColOverMax(State* state) {
    if (state->col > state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
}

void sanityCheckRowColOutOfBounds(State* state) {
    if (state->row >= state->data.size()) {
        state->row = state->data.size() - 1;
    }
    if (state->mode == VISUAL || state->mode == TYPING) {
        fixColOverMax(state);
    }
}
