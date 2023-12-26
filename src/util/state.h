#pragma once

#include <string>
#include <vector>
#include "helper.h"
#include "modes.h"

class State {
    public:
        char* filename;
        std::vector<std::string> data;
        uint row;
        uint col;
        std::string prevKeys;
        std::string status;
        Mode mode;
        State(char* filename);
};
