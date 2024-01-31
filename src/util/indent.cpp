#include <vector>
#include <string>
#include "indent.h"

uint getNumLeadingSpaces(State* state, uint row) {
    uint numSpaces = 0;
    for (uint i = 0; i < state->data[row].length(); i++) {
        if (state->data[row][i] == ' ') {
            numSpaces++;
        } else {
            break;
        }
    }
    return numSpaces;
}
