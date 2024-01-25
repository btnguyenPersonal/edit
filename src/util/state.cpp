#include <string>
#include <vector>
#include "helper.h"
#include "state.h"
#include "modes.h"
#include "visualType.h"
#include "state.h"

unsigned int State::maxX = 0;
unsigned int State::maxY = 0;

void State::resetState(const char* filename) {
    bool found = false;
    for (uint i = 0; i < this->archives.size(); i++) {
        if (this->archives[i].filename == this->filename) {
            this->archives[i].data = this->data;
            this->archives[i].previousState = this->previousState;
            this->archives[i].history = this->history;
            this->archives[i].historyPosition = this->historyPosition;
            this->archives[i].windowPosition = this->windowPosition;
            this->archives[i].row = this->row;
            this->archives[i].col = this->col;
            found = true;
            break;
        }
    }
    if (!found) {
        this->archives.push_back({
            this->filename,
            this->data,
            this->previousState,
            this->history,
            this->historyPosition,
            this->windowPosition,
            this->row,
            this->col,
        });
    }
    for (uint i = 0; i < this->archives.size(); i++) {
        if (this->archives[i].filename == std::string(filename)) {
            auto archive = this->archives[i];
            this->filename = std::string(filename);
            this->data = archive.data;
            this->previousState = archive.previousState;
            this->history = archive.history;
            this->historyPosition = archive.historyPosition;
            this->windowPosition = archive.windowPosition;
            this->row = archive.row;
            this->col = archive.col;
            this->mode = SHORTCUTS;
            return;
        }
    }
    this->filename = std::string(filename);
    this->data = readFile(filename);
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    this->historyPosition = -1;
    this->windowPosition = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->commandLineQuery = std::string("");
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = SHORTCUTS;
}

State::State() {
    this->harpoonFiles = std::vector<std::string>();
    this->harpoonIndex = 0;
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    this->grepOutput = std::vector<grepMatch>();
    this->findFileOutput = std::vector<std::filesystem::path>();
    this->historyPosition = -1;
    this->windowPosition = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->searchQuery = std::string("");
    this->replaceQuery = std::string("");
    this->commandLineQuery = std::string("");
    this->findFileQuery = std::string("");
    this->findFileSelection = 0;
    this->grepQuery = std::string("");
    this->grepSelection = 0;
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = FINDFILE;
    this->dotCommand = std::string("");
    this->macroCommand = std::string("");
    this->playingCommand = false;
    this->recording = false;
    this->dontRecordKey = false;
}

State::State(const char* filename) {
    this->harpoonFiles = std::vector<std::string>();
    this->harpoonIndex = 0;
    this->filename = std::string(filename);
    this->data = readFile(filename);
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    this->grepOutput = std::vector<grepMatch>();
    this->findFileOutput = std::vector<std::filesystem::path>();
    this->historyPosition = -1;
    this->windowPosition = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->searchQuery = std::string("");
    this->replaceQuery = std::string("");
    this->commandLineQuery = std::string("");
    this->findFileQuery = std::string("");
    this->findFileSelection = 0;
    this->grepQuery = std::string("");
    this->grepSelection = 0;
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = SHORTCUTS;
    this->dotCommand = std::string("");
    this->macroCommand = std::string("");
    this->playingCommand = false;
    this->recording = false;
    this->dontRecordKey = false;
}

void State::setMaxYX(int y, int x) {
    State::maxY = (unsigned int) y;
    State::maxX = (unsigned int) x;
}
