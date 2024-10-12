#include "clipboard.h"
#include "../keybinds/sendVisualKeys.h"
#include "helper.h"
#include "indent.h"
#include "state.h"
#include <array>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string getFromClipboard() {
    std::string command;
#ifdef __APPLE__
    command = "pbpaste";
#elif defined(__linux__)
    command = "xclip -selection clipboard -o";
#else
#error "Platform not supported"
#endif

    std::string result;
    std::array<char, 256> buffer;
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe)
        throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    int status = pclose(pipe);
    if (status != 0 || result.empty()) {
        return {""};
    }
    return result;
}

std::string escapeForShell(const std::string& s) {
    std::ostringstream oss;
    for (auto c : s) {
        if (c == '\'') {
            oss << "'\\''";
        } else {
            oss << c;
        }
    }
    return oss.str();
}

void copyFileToClipboard(State* state, const std::string& filePath) {
#ifdef __APPLE__
    // TODO
#elif defined(__linux__)
    auto absolutePath = std::filesystem::absolute(filePath);
    auto path = std::string("file://") + escapeForShell(absolutePath.string());
    std::string command = "xclip -selection clipboard -t text/uri-list";
    try {
        FILE* pipe = popen(command.c_str(), "w");
        if (pipe != nullptr) {
            fwrite(path.c_str(), sizeof(char), path.size(), pipe);
            pclose(pipe);
        }
        state->status = filePath;
    } catch (const std::exception& e) {
        state->status = std::string("file copy failed") + filePath;
    }
#else
#error "Platform not supported"
#endif
}

void pasteFromClipboardVisual(State* state) {
    std::string result = getFromClipboard();
    std::vector<std::string> clip;
    std::stringstream ss(result);
    std::string line;
    while (std::getline(ss, line)) {
        clip.push_back(line);
    }
    bool isClipLine = !result.empty() && result.back() == '\n';
    if (state->visualType == LINE && !isClipLine) {
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        insertEmptyLine(state);
    } else if (state->visualType == NORMAL && isClipLine) {
        auto pos = changeInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col);
        state->data.insert(state->data.begin() + state->row + 1, current.substr(state->col));
        state->row += 1;
        state->col = 0;
        indentLine(state);
    } else {
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
    }
    fixColOverMax(state);
    if (state->data.size() == 0) {
        for (unsigned int i = 0; i < clip.size(); i++) {
            state->data.push_back(clip[i]);
        }
    } else if (!result.empty() && result.back() == '\n') {
        for (int i = 0; i < (int)clip.size(); i++) {
            state->data.insert(state->data.begin() + i + state->row, clip[i]);
        }
    } else if (clip.size() > 0) {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col);
        state->data[state->row] += clip[0];
        int lastRow = state->row;
        for (int i = 1; i < (int)clip.size(); i++) {
            int r = i + state->row;
            state->data.insert(state->data.begin() + r, clip[i]);
            lastRow = r;
        }
        state->data[lastRow] += safeSubstring(current, state->col);
    }
}

void pasteFromClipboard(State* state) {
    fixColOverMax(state);
    std::string result = getFromClipboard();
    std::vector<std::string> clip;
    std::stringstream ss(result);
    std::string line;
    while (std::getline(ss, line)) {
        clip.push_back(line);
    }
    if (state->data.size() == 0) {
        for (unsigned int i = 0; i < clip.size(); i++) {
            state->data.push_back(clip[i]);
        }
    } else if (!result.empty() && result.back() == '\n') {
        for (int i = 0; i < (int)clip.size(); i++) {
            state->data.insert(state->data.begin() + i + state->row, clip[i]);
        }
    } else if (clip.size() > 0) {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col);
        state->data[state->row] += clip[0];
        int lastRow = state->row;
        for (int i = 1; i < (int)clip.size(); i++) {
            int r = i + state->row;
            state->data.insert(state->data.begin() + r, clip[i]);
            lastRow = r;
        }
        state->data[lastRow] += current.substr(state->col);
    }
}

void pasteFromClipboardAfter(State* state) {
    fixColOverMax(state);
    std::string result = getFromClipboard();
    std::vector<std::string> clip;
    std::stringstream ss(result);
    std::string line;
    while (std::getline(ss, line)) {
        clip.push_back(line);
    }
    if (!result.empty() && result.back() == '\n') {
        for (int i = 0; i < (int)clip.size(); i++) {
            state->data.insert(state->data.begin() + i + state->row + 1, clip[i]);
        }
    } else if (clip.size() > 0) {
        std::string current = state->data[state->row];
        int breakCol = state->col;
        if (state->col + 1 <= state->data[state->row].length()) {
            breakCol = state->col + 1;
        }
        state->data[state->row] = current.substr(0, breakCol);
        state->data[state->row] += clip[0];
        int lastRow = state->row;
        for (int i = 1; i < (int)clip.size(); i++) {
            int r = i + state->row;
            state->data.insert(state->data.begin() + r, clip[i]);
            lastRow = r;
        }
        state->data[lastRow] += current.substr(breakCol);
    }
}

void copyToClipboard(const std::string& clip) {
#ifdef __APPLE__
    FILE* pipe = popen("pbcopy", "w");
#elif defined(__linux__)
    FILE* pipe = popen("xclip -selection clipboard", "w");
#else
#error "OS not supported"
#endif
    if (pipe != nullptr) {
        fwrite(clip.c_str(), sizeof(char), clip.size(), pipe);
        pclose(pipe);
    }
}
