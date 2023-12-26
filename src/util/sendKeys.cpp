#include <string>
#include <vector>
#include "render.h"
#include "state.h"

void sendKeys(State* state, char c) {
    if (c == 'k') {
        state->data.push_back(std::string(1, c));
    }
}
