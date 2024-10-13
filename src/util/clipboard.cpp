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

std::string getFileFromClipboard() {
#ifdef __APPLE__
    // FILE* pipe = popen("osascript -e 'get the clipboard as «class furl»'", "r");
#elif defined(__linux__)
    FILE* pipe = popen("xclip -selection clipboard -o -t text/uri-list", "r");
#else
#error "Platform not supported"
#endif

    if (!pipe) {
        throw std::runtime_error("Failed to open pipe");
    }

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }
    pclose(pipe);

    // Remove any trailing newline
    if (!result.empty() && result[result.length()-1] == '\n') {
        result.erase(result.length()-1);
    }

    return result;
}

std::filesystem::path getUniqueFilePath(const std::filesystem::path& basePath) {
    if (!std::filesystem::exists(basePath)) {
        return basePath;
    }

    std::filesystem::path stem = basePath.stem();
    std::filesystem::path extension = basePath.extension();
    std::filesystem::path directory = basePath.parent_path();

    for (int i = 1; ; ++i) {
        std::filesystem::path newPath = directory / (stem.string() + " (" + std::to_string(i) + ")" + extension.string());
        if (!std::filesystem::exists(newPath)) {
            return newPath;
        }
    }
}

void pasteFileFromClipboard(State* state, const std::string& destFolder) {
    try {
        std::string sourcePathStr = getFileFromClipboard();
        state->status = sourcePathStr;
#ifdef __APPLE__
        // TODO
        std::filesystem::path sourcePath(sourcePathStr);
#elif defined(__linux__)
        std::filesystem::path sourcePath(sourcePathStr.substr(7));
#else
#error "Platform not supported"
#endif
        if (!std::filesystem::exists(sourcePath)) {
            state->status = std::string("Error: file does not exist: ") + sourcePath.string();
            return;
        }

        std::filesystem::path destPath = std::filesystem::path(destFolder) / sourcePath.filename();
        destPath = getUniqueFilePath(destPath);

        std::filesystem::copy(sourcePath, destPath, std::filesystem::copy_options::overwrite_existing);
    } catch (const std::exception& e) {
        state->status = "File paste failed: " + std::string(e.what());
    }
}

void copyFileToClipboard(State* state, const std::string& filePath) {
#ifdef __APPLE__
    // TODO
#elif defined(__linux__)
    auto path = std::string("file://") + escapeForShell(filePath);
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
