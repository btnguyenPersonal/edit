#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/modes.h"

void sendTypingKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    }
}