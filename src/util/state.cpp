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

void State::changeFile(std::string filename) {
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
        if (this->archives[i].filename == std::string(filename)) {
            auto archive = this->archives[i];
            this->filename = std::string(filename);
            this->data = readFile(filename);
            this->commentSymbol = getCommentSymbol(filename);
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
    auto data = readFile(filename);
    this->filename = std::string(filename);
    this->data = data;
    this->previousState = data;
    this->commentSymbol = getCommentSymbol(filename);
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
    this->pushFileStack(this->filename);
    this->pushFileStack(filename);
    this->fileStackIndex = this->fileStack.size() - 1;
    if (!std::filesystem::is_regular_file(filename.c_str())) {
        this->status = "file not found: " + filename;
        exit(1);
    }
    this->changeFile(filename);
}

State::State() {
    this->autosave = true;
    this->mark = {"", 0};
    this->matching = {0, 0};
    this->searching = false;
    this->dontSave = false;
    this->replacing = false;
    this->jumplist = {0, std::vector<Position>()};
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
    if (!std::filesystem::is_regular_file(filename.c_str())) {
        endwin();
        std::cout << "file not found: " << filename << std::endl;
        exit(1);
    }
    auto data = readFile(filename.c_str());
    this->filename = std::string(filename);
    this->data = data;
    this->previousState = data;
    this->commentSymbol = getCommentSymbol(filename);
    this->mode = SHORTCUTS;
    this->fileStack = {filename};
    this->fileStackIndex = 0;
}

void State::setMaxYX(int y, int x) {
    State::maxY = (unsigned int)y;
    State::maxX = (unsigned int)x;
}
