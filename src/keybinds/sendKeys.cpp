#include "sendKeys.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/keys.h"
#include "sendBlameKeys.h"
#include "sendFileExplorerKeys.h"
#include "sendCommandLineKeys.h"
#include "sendFindFileKeys.h"
#include "sendGrepKeys.h"
#include "sendMultiCursorKeys.h"
#include "sendSearchKeys.h"
#include "sendShortcutKeys.h"
#include "sendTypingKeys.h"
#include "sendVisualKeys.h"
#include <ncurses.h>

void sendKeys(State *state, int32_t c)
{
	if (c == KEY_MOUSE) {
		state->keys.push_back({ state->mode, getEscapedChar('\xFF', false) });
	} else if (c == KEY_BACKSPACE) {
		state->keys.push_back({ state->mode, getEscapedChar(127, false) });
	} else {
		state->keys.push_back({ state->mode, getEscapedChar(c, false) });
	}
	state->showGrep = false;
	state->status = std::string("");
	state->searchFail = false;
	state->showFileStack = false;
	state->searching = state->mode == SEARCH;
	calcWindowBounds();
	switch (state->mode) {
	case SHORTCUTS:
		sendShortcutKeys(state, c);
		break;
	case TYPING:
		sendTypingKeys(state, c);
		break;
	case VISUAL:
		sendVisualKeys(state, c, false);
		break;
	case COMMANDLINE:
		sendCommandLineKeys(state, c);
		break;
	case FINDFILE:
		sendFindFileKeys(state, c);
		break;
	case GREP:
		sendGrepKeys(state, c);
		break;
	case SEARCH:
		sendSearchKeys(state, c);
		break;
	case FILEEXPLORER:
		sendFileExplorerKeys(state, c);
		break;
	case BLAME:
		sendBlameKeys(state, c);
		break;
	case MULTICURSOR:
		sendMultiCursorKeys(state, c);
		break;
	case NAMING:
		state->status = "Error: in NAMING mode outside of naming loop";
		break;
	}
}
