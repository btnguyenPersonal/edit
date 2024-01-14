#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"

void sendFindFileKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->findFileQuery = std::string("");
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->findFileQuery += c;
    } else if (c == 13) { // ENTER
        state->findFileQuery = std::string("");
        state->mode = SHORTCUTS;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
