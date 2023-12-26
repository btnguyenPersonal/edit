#include <string>
#include <vector>
#include "helper.h"
#include "state.h"

State::State(char* filename) {
    this->filename = filename;
    this->data = readFile(filename);
}
