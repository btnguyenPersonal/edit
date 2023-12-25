#pragma once

#include <string>
#include <vector>
#include "helper.cpp"

class State {
    public:
        char* filename;
        std::vector<std::string> data;
        State(char* filename) {
            this->filename = filename;
            this->data = readFile(filename);
        }
};