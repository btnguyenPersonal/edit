#pragma once

#include "modes.h"
#include "visualType.h"
#include "fileExplorerNode.h"
#include <filesystem>
#include <string>
#include <vector>

struct grepMatch {
    std::filesystem::path path;
    int lineNum;
    std::string line;

    grepMatch(const std::filesystem::path& p, int num, const std::string& l) : path(p), lineNum(num), line(l) {}
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

struct Mark {
    std::string filename;
    unsigned int mark;
};

struct Jumplist {
    unsigned int index;
    std::vector<Position> list;
};

struct Archive {
    std::string filename;
    std::vector<std::string> previousState;
    std::vector<std::vector<diffLine>> history;
    int historyPosition;
    struct Position windowPosition;
    unsigned int row;
    unsigned int col;
    Jumplist jumplist;
};

struct Query {
    std::string query;
    unsigned int cursor;
    unsigned int selection;
};

class State {
public:
    bool fileExplorerOpen;
    int fileExplorerSize;
    int fileExplorerIndex;
    int fileExplorerWindowLine;
    Position matching;
    bool autosave;
    bool searchFail;
    Jumplist jumplist;
    std::vector<Archive> archives;
    std::vector<std::string> blame;
    std::vector<std::string> harpoonFiles;
    unsigned int harpoonIndex;
    std::string filename;
    std::vector<std::string> data;
    std::vector<std::string> previousState;
    std::vector<std::string> fileStack;
    unsigned int fileStackIndex;
    std::vector<std::vector<diffLine>> history;
    int historyPosition;
    bool searching;
    bool replacing;
    unsigned int indent;
    Mark mark;
    struct Position windowPosition;
    static unsigned int maxX;
    static unsigned int maxY;
    VisualType visualType;
    struct Position visual;
    unsigned int row;
    unsigned int col;
    std::string commentSymbol;
    Query name;
    Query search;
    Query replace;
    Query commandLine;
    std::string dotCommand;
    std::string macroCommand;
    bool playingCommand;
    std::string motion;
    bool recording;
    bool dontRecordKey;
    Query findFile;
    std::vector<std::filesystem::path> findFileOutput;
    Query grep;
    std::vector<grepMatch> grepOutput;
    std::string prevKeys;
    std::string lastLoggingVar;
    std::string status;
    bool dontSave;
    bool selectAll;
    bool showFileStack;
    FileExplorerNode* fileExplorer;
    Mode mode;
    State();
    State(std::string filename);
    State(std::vector<std::string> data);
    void resetState(std::string filename);
    void pushFileStack(std::string filename);
    void changeFile(std::string filename);
    static void setMaxYX(int y, int x);
};
