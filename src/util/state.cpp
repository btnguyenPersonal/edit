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
    // keep search & replaceQuery
    // keep grep state
    // keep findFile state
    // TODO save file state in archive, and see if can retrieve file state from archive
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
