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
    uint lineNum;
    bool add; // true for add, false for delete
    std::string line;
};

struct Position {
    uint row;
    uint col;
};

struct WordPosition {
    uint min;
    uint max;
};

struct Archive {
    std::string filename;
    std::vector<std::string> previousState;
    std::vector<std::vector<diffLine>> history;
    int historyPosition;
    struct Position windowPosition;
    uint row;
    uint col;
};

class State {
    public:
        std::vector<Archive> archives;
        std::vector<std::string> harpoonFiles;
        uint harpoonIndex;
        std::string filename;
        std::vector<std::string> data;
        std::vector<std::string> previousState;
        std::vector<std::string> fileStack;
        uint fileStackIndex;
        std::vector<std::vector<diffLine>> history;
        int historyPosition;
        bool searching;
        bool replacing;
        uint indent;
        struct Position windowPosition;
        static uint maxX;
        static uint maxY;
        VisualType visualType;
        struct Position visual;
        uint row;
        uint col;
        std::string commentSymbol;
        std::string searchQuery;
        std::string replaceQuery;
        std::string commandLineQuery;
        std::string dotCommand;
        std::string macroCommand;
        bool playingCommand;
        bool motionComplete;
        std::string motion;
        bool recording;
        bool dontRecordKey;
        std::string findFileQuery;
        uint findFileSelection;
        std::vector<std::filesystem::path> findFileOutput;
        std::string grepQuery;
        uint grepSelection;
        std::vector<grepMatch> grepOutput;
        std::string prevKeys;
        std::string status;
        bool showFileStack;
        Mode mode;
        State();
        State(std::string filename);
        void resetState(std::string filename);
        void pushFileStack(std::string filename);
        void changeFile(std::string filename);
        static void setMaxYX(int y, int x);
};
