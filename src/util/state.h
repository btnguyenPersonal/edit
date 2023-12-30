#pragma once

#include <string>
#include <vector>
#include "modes.h"

class State {
    public:
        char* filename;
        std::vector<std::string> data;
        uint windowPosition;
        uint row;
        uint col;
        std::string commandLineQuery;
        std::string prevKeys;
        std::string status;
        Mode mode;
        State(char* filename);
};
