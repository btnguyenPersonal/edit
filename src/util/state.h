#pragma once

#include <string>
#include <vector>
#include "helper.h"

class State {
    public:
        char* filename;
        std::vector<std::string> data;
        State(char* filename);
};
