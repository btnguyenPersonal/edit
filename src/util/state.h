#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "modes.h"
#include "visualType.h"

struct diffLine {
    uint lineNum;
    bool add; // true for add, false for delete
    std::string line;
};

struct Position {
    uint row;
    uint col;
};

class State {
    public:
        const char* filename;
        std::vector<std::string> data;
        std::vector<std::string> previousState;
        std::vector<std::vector<diffLine>> history;
        int historyPosition;
        uint indent;
        uint windowPosition;
        static uint maxX;
        static uint maxY;
        VisualType visualType;
        struct Position visual;
        uint row;
        uint col;
        std::string commandLineQuery;
        std::string findFileQuery;
        uint findFileSelection;
        std::vector<std::filesystem::path> findFileOutput;
        std::string prevKeys;
        std::string status;
        Mode mode;
        State(const char* filename);
        static void setMaxYX(int y, int x);
};
