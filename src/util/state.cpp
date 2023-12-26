#include <string>
#include <vector>
#include "helper.h"
#include "state.h"
#include "modes.h"

State::State(char* filename) {
    this->filename = filename;
    this->data = readFile(filename);
    this->row = 0;
    this->col = 0;
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = SHORTCUTS;
}
