#include <string>
#include <vector>
#include "helper.h"
#include "state.h"
#include "modes.h"
#include "visualType.h"

uint State::maxX = 0;
uint State::maxY = 0;

State::State(char* filename) {
    this->filename = filename;
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
    this->findFileQuery = std::string("");
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = SHORTCUTS;
}

void State::setMaxYX(int y, int x) {
    State::maxY = (uint) y;
    State::maxX = (uint) x;
}
