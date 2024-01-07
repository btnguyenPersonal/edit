#pragma once

#include <string>
#include <vector>
#include "modes.h"
#include "visualType.h"

struct Position {
    uint row;
    uint col;
};

class State {
    public:
        char* filename;
        std::vector<std::string> data;
        uint indent;
        uint windowPosition;
        static uint maxX;
        static uint maxY;
        VisualType visualType;
        struct Position visual;
        uint row;
        uint col;
        std::string commandLineQuery;
        std::string prevKeys;
        std::string status;
        Mode mode;
        State(char* filename);
        static void setMaxYX(int y, int x);
};
