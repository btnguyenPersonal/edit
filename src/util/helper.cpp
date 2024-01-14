#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include <curses.h>
#include "state.h"

char ctrl(char c) {
    return c - 'a' + 1;
}

bool isAlphaNumeric(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool filePathContainsSubstring(const std::filesystem::path& filePath, const std::string& query) {
    // TODO make fzf
    return filePath.string().find(query) != std::string::npos;
}

bool shouldIgnoreFile(const std::filesystem::path& path) {
    std::vector<std::string> ignoreList = {".git", "node_modules"};
    for (uint i = 0; i < ignoreList.size(); i++) {
        if (path.string().find(ignoreList[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> findFilesWithSubstring(const std::filesystem::path& dir_path, const std::string& query) {
    std::vector<std::filesystem::path> matching_files;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (shouldIgnoreFile(entry.path())) {
            continue;
        }

        if (std::filesystem::is_regular_file(entry) && filePathContainsSubstring(entry.path(), query)) {
            matching_files.push_back(entry.path());
        }
    }

    return matching_files;
}

void generateFindFileOutput(State* state) {
    state->findFileOutput = findFilesWithSubstring(std::filesystem::current_path(), state->findFileQuery);
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
    std::ostream_iterator<std::string> output_iterator(file, "\n");
    std::copy(std::begin(data), std::end(data), output_iterator);
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
    return state->row < state->windowPosition || (int) state->row - (int) state->windowPosition > ((int) state->maxY - 2);
}

void centerScreen(State* state) {
    if (state->row < state->maxY / 2) {
        state->windowPosition = 0;
    } else {
        state->windowPosition = state->row - state->maxY / 2;
    }
}

void upHalfScreen(State* state) {
    for (uint i = 0; i < state->maxY / 2; i++) {
        if (state->row > 0) {
            state->row -= 1;
            state->windowPosition -= 1;
        }
    }
}

void downHalfScreen(State* state) {
    for (uint i = 0; i < state->maxY / 2; i++) {
        if (state->row < state->data.size() - 1) {
            state->row += 1;
            state->windowPosition += 1;
        }
    }
}

void up(State* state) {
    if (state->row > 0) {
        state->row -= 1;
    }
    if (state->row < state->windowPosition) {
        state->windowPosition -= 1;
    }
}

void down(State* state) {
    if (state->row < state->data.size() - 1) {
        state->row += 1;
    }
    if ((int) state->row - (int) state->windowPosition > ((int) state->maxY - 2)) {
        state->windowPosition += 1;
    }
}

void left(State* state) {
    if (state->col > 0) {
        state->col -= 1;
    }
}

void right(State* state) {
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

bool isMotionCompleted(State* state) {
    return state->prevKeys == "";
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
            state->prevKeys = "";
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

void sanityCheckRowColOutOfBounds(State* state) {
    if (state->data.size() == 0) {
        state->data.push_back("");
    }
    if (state->row >= state->data.size()) {
        state->row = state->data.size() - 1;
    }
    if ((state->mode == VISUAL || state->mode == TYPING) && state->col > state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
}
