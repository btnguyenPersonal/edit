#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "modes.h"
#include "visualType.h"

struct grepMatch {
    std::filesystem::path path;
    int lineNum;
    std::string line;
};

struct diffLine {
    unsigned int lineNum;
    bool add; // true for add, false for delete
    std::string line;
};

struct Position {
    unsigned int row;
    unsigned int col;
};

struct WordPosition {
    unsigned int min;
    unsigned int max;
};

class State {
    public:
        std::string filename;
        std::vector<std::string> data;
        std::vector<std::string> previousState;
        std::vector<std::vector<diffLine>> history;
        int historyPosition;
        unsigned int indent;
        unsigned int windowPosition;
        static unsigned int maxX;
        static unsigned int maxY;
        VisualType visualType;
        struct Position visual;
        unsigned int row;
        unsigned int col;
        std::string searchQuery;
        std::string replaceQuery;
        std::string commandLineQuery;
        std::string findFileQuery;
        unsigned int findFileSelection;
        std::vector<std::filesystem::path> findFileOutput;
        std::string grepQuery;
        unsigned int grepSelection;
        std::vector<grepMatch> grepOutput;
        std::string prevKeys;
        std::string status;
        Mode mode;
        State();
        State(const char* filename);
        void resetState(const char* filename);
        static void setMaxYX(int y, int x);
};
