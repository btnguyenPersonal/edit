#include "sendShortcutKeys.h"
#include "sendTypingKeys.h"
#include "sendCommandLineKeys.h"
#include "sendVisualKeys.h"
#include "../util/state.h"
#include "../util/modes.h"

void sendKeys(State* state, char c) {
    if (state->mode == TYPING) {
        sendTypingKeys(state, c);
    } else if (state->mode == VISUAL) {
        sendVisualKeys(state, c);
    } else if (state->mode == SHORTCUTS) {
        sendShortcutKeys(state, c);
    } else if (state->mode == COMMANDLINE) {
        sendCommandLineKeys(state, c);
    }
}