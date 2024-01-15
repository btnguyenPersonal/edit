#include <string>
#include <vector>
#include "helper.h"
#include "state.h"
#include "modes.h"
#include "visualType.h"
#include "state.h"

uint State::maxX = 0;
uint State::maxY = 0;

void State::resetState(const char* filename) {
    this->filename = std::string(filename);
    this->data = readFile(filename);
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    // keep grep state
    // this->grepQuery = std::string("");
    // this->grepSelection = 0;
    // this->grepOutput = std::vector<grepMatch>();
    this->historyPosition = -1;
    this->windowPosition = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->commandLineQuery = std::string("");
    // keep findFile state
    // this->findFileQuery = std::string("");
    // this->findFileSelection = 0;
    // this->findFileOutput = std::vector<std::filesystem::path>();
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = SHORTCUTS;
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
    this->commandLineQuery = std::string("");
    this->findFileQuery = std::string("");
    this->findFileSelection = 0;
    this->grepQuery = std::string("");
    this->grepSelection = 0;
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = SHORTCUTS;
}

void State::setMaxYX(int y, int x) {
    State::maxY = (uint) y;
    State::maxX = (uint) x;
}
