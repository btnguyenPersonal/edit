#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include <curses.h>
#include "state.h"

char ctrl(char c) {
    return c - 'a' + 1;
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
    while (std::getline(file, str)) {
        file_contents.push_back(str);
    }
    return file_contents;
}

bool isWindowPositionInvalid(State state) {
    return state.row < state.windowPosition || (int) state.row - (int) state.windowPosition > ((int) state.maxY - 2);
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

void calcWindowBounds() {
    int y, x;
    getmaxyx(stdscr, y, x);
    State::setMaxYX(y, x);
}
