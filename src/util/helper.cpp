#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include "state.h"

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

bool isWindowPositionInvalid(State state, int maxY) {
    return state.row < state.windowPosition || ((int) state.row) - ((int) state.windowPosition) > (maxY - 2);
}

void centerScreen(State* state, int maxY) {
    if (state->row < (uint) maxY / 2) {
        state->windowPosition = 0;
    } else {
        state->windowPosition = state->row - (uint) maxY / 2;
    }
}
