#include <sstream>
#include <string>
#include <array>
#include <iterator>
#include <vector>
#include "state.h"
#include "clipboard.h"

std::vector<std::string> getFromClipboard() {
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
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    pclose(pipe);

    std::vector<std::string> output;
    std::stringstream ss(result);
    std::string line;
    while (std::getline(ss, line)) {
        output.push_back(line);
    }

    return output;
}

void pasteFromClipboard(State* state) {
    std::vector<std::string> clip = getFromClipboard();
    if (clip.back() == "") {
        clip.pop_back();
        for (int i = 0; i < (int) clip.size(); i++) {
            state->data.insert(state->data.begin() + i + state->row, clip[i]);
        }
    } else if (clip.size() > 0) {
        std::string current = state->data[state->row];

        // break up current line
        state->data[state->row] = current.substr(0, state->col);

        // insert first line of clip
        state->data[state->row] += clip[0];
        int lastRow = state->row;

        // insert other lines of clip
        for (int i = 1; i < (int) clip.size(); i++) {
            int r = i + state->row;
            state->data.insert(state->data.begin() + r, clip[i]);
            lastRow = r;
        }

        // insert last part of broken line
        state->data[lastRow] += current.substr(state->col);
    }
}

void pasteFromClipboardAfter(State* state) {
    std::vector<std::string> clip = getFromClipboard();
    if (clip.back() == "") {
        clip.pop_back();
        for (int i = 0; i < (int) clip.size(); i++) {
            state->data.insert(state->data.begin() + i + state->row + 1, clip[i]);
        }
    } else if (clip.size() > 0) {
        std::string current = state->data[state->row];

        // break up current line
        int breakCol = state->col;
        if (state->col + 1 < state->data[state->row].length()) {
            breakCol = state->col + 1;
        }
        state->data[state->row] = current.substr(0, breakCol);

        // insert first line of clip
        state->data[state->row] += clip[0];
        int lastRow = state->row;

        // insert other lines of clip
        for (int i = 1; i < (int) clip.size(); i++) {
            int r = i + state->row;
            state->data.insert(state->data.begin() + r, clip[i]);
            lastRow = r;
        }

        // insert last part of broken line
        state->data[lastRow] += current.substr(breakCol);
    }
}

void copyToClipboard(std::string s) {
    std::string command;
    #ifdef __APPLE__
        command = "echo \"" + s + "\" | pbcopy";
    #elif defined(__linux__)
        command = "echo \"" + s + "\" | xclip -selection clipboard";
    #else
        #error "Platform not supported"
    #endif
    std::system(command.c_str());
}
