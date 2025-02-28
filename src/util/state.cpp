#include "state.h"
#include "helper.h"
#include "modes.h"
#include "state.h"
#include "visualType.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

unsigned int State::maxX = 0;
unsigned int State::maxY = 0;

std::string normalizeFilename(std::string filename) {
    std::string current_path = std::filesystem::current_path().string() + "/";
    if (filename.substr(0, current_path.length()) == current_path) {
        return filename.substr(current_path.length());
    } else {
        return filename;
    }
}

void State::changeFile(std::string filename) {
    auto normalizedFilename = normalizeFilename(filename);
    if (this->fileExplorerOpen) {
        this->fileExplorerIndex = this->fileExplorer->expand(normalizedFilename);
        centerFileExplorer(this);
    }
    bool found = false;
    for (unsigned int i = 0; i < this->archives.size(); i++) {
        if (this->archives[i].filename == this->filename) {
            this->archives[i].previousState = this->previousState;
            this->archives[i].history = this->history;
            this->archives[i].historyPosition = this->historyPosition;
            this->archives[i].windowPosition = this->windowPosition;
            this->archives[i].row = this->row;
            this->archives[i].col = this->col;
            this->archives[i].jumplist = this->jumplist;
            found = true;
            break;
        }
    }
    if (!found) {
        this->archives.push_back({
            this->filename,
            this->previousState,
            this->history,
            this->historyPosition,
            this->windowPosition,
            this->row,
            this->col,
            this->jumplist,
        });
    }
    for (unsigned int i = 0; i < this->archives.size(); i++) {
        if (this->archives[i].filename == normalizedFilename) {
            auto archive = this->archives[i];
            this->filename = normalizedFilename;
            this->data = readFile(normalizedFilename);
            this->commentSymbol = getCommentSymbol(normalizedFilename);
            this->previousState = archive.previousState;
            this->history = archive.history;
            this->historyPosition = archive.historyPosition;
            this->windowPosition = archive.windowPosition;
            this->row = archive.row;
            this->col = archive.col;
            this->jumplist = archive.jumplist;
            this->mode = SHORTCUTS;
            return;
        }
    }
    auto data = readFile(normalizedFilename);
    this->filename = normalizedFilename;
    this->data = data;
    this->previousState = data;
    this->commentSymbol = getCommentSymbol(normalizedFilename);
    this->history = std::vector<std::vector<diffLine>>();
    this->historyPosition = -1;
    this->windowPosition.row = 0;
    this->windowPosition.col = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->commandLine.query = std::string("");
    this->selectAll = false;
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->searchFail = false;
    this->motion = std::string("");
    this->mode = SHORTCUTS;
}

void State::pushFileStack(std::string filename) {
    if (filename != "") {
        for (auto it = this->fileStack.begin(); it != this->fileStack.end();) {
            if (*it == filename) {
                it = this->fileStack.erase(it);
            } else {
                ++it;
            }
        }
        this->fileStack.push_back(filename);
    }
}

void State::resetState(std::string filename) {
    auto normalizedFilename = normalizeFilename(filename);
    this->pushFileStack(this->filename);
    this->pushFileStack(normalizedFilename);
    this->fileStackIndex = this->fileStack.size() - 1;
    if (!std::filesystem::is_regular_file(normalizedFilename.c_str())) {
        this->status = "file not found: " + normalizedFilename;
        exit(1);
    }
    this->changeFile(normalizedFilename);
}

State::State() {
    this->fileExplorer = new FileExplorerNode(std::filesystem::current_path());
    this->fileExplorer->open();
    this->fileExplorerOpen = true;
    this->fileExplorerSize = 40;
    this->fileExplorerWindowLine = 0;
    this->fileExplorerIndex = 0;
    this->autosave = true;
    this->buildDir = ".";
    this->mark = {"", 0};
    this->matching = {0, 0};
    this->searching = false;
    this->dontSave = false;
    this->replacing = false;
    this->jumplist = {0, std::vector<Position>()};
    this->buildErrorIndex = 0;
    this->buildErrors = std::vector<std::string>();
    this->blame = std::vector<std::string>();
    this->harpoonFiles = std::vector<std::string>();
    this->harpoonIndex = 0;
    this->data = std::vector<std::string>();
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    this->grepOutput = std::vector<grepMatch>();
    this->findFileOutput = std::vector<std::filesystem::path>();
    this->historyPosition = -1;
    this->windowPosition.row = 0;
    this->windowPosition.col = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->search = {std::string(""), 0, 0};
    this->replace = {std::string(""), 0, 0};
    this->commandLine = {std::string(""), 0, 0};
    this->findFile = {std::string(""), 0, 0};
    this->grep = {std::string(""), 0, 0};
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->searchFail = false;
    this->mode = FINDFILE;
    this->dotCommand = std::string("");
    this->macroCommand = std::string("");
    this->playingCommand = false;
    this->recording = false;
    this->selectAll = false;
    this->dontRecordKey = false;
    this->fileStack = std::vector<std::string>();
    this->showFileStack = false;
    this->fileStackIndex = 0;
    this->lastLoggingVar = std::string("");
    this->motion = std::string("");
}

State::State(std::vector<std::string> data) : State() {
    this->filename = std::string("test");
    this->data = data;
    this->previousState = data;
    this->commentSymbol = "";
    this->mode = SHORTCUTS;
    this->fileStack = {filename};
    this->fileStackIndex = 0;
    this->dontSave = true;
}

State::State(std::string filename) : State() {
    auto normalizedFilename = normalizeFilename(filename);
    if (!std::filesystem::is_regular_file(normalizedFilename.c_str())) {
        endwin();
        std::cout << "file not found: " << normalizedFilename << std::endl;
        exit(1);
    }
    auto data = readFile(normalizedFilename.c_str());
    this->filename = std::string(normalizedFilename);
    this->data = data;
    this->previousState = data;
    this->commentSymbol = getCommentSymbol(normalizedFilename);
    this->mode = SHORTCUTS;
    this->fileStack = {normalizedFilename};
    this->fileStackIndex = 0;
}

void State::setMaxYX(int y, int x) {
    State::maxY = (unsigned int)y;
    State::maxX = (unsigned int)x;
}
