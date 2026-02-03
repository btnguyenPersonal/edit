#include "sendKeys.h"
#include "../util/assert.h"
#include "../util/display.h"
#include "../util/keys.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "sendBlameKeys.h"
#include "sendCommandLineKeys.h"
#include "sendDiffKeys.h"
#include "sendFileExplorerKeys.h"
#include "sendFindKeys.h"
#include "sendGrepKeys.h"
#include "sendMultiCursorKeys.h"
#include "sendNormalKeys.h"
#include "sendSearchKeys.h"
#include "sendTypingKeys.h"
#include "sendVisualKeys.h"
#include <ncurses.h>

void sendKeys(State *state, int32_t c) {
	if (state->mode != FIND && state->mode != GREP) {
		assert(state->file);
	}
	if (c == KEY_MOUSE) {
		state->keys.push_back({state->mode, getEscapedChar('\xFF', false)});
	} else {
		state->keys.push_back({state->mode, getEscapedChar(c, false)});
	}
	state->showGrep = false;
	state->status = std::string("");
	state->searchFail = false;
	state->showFileStack = false;
	state->searching = state->mode == SEARCH;
	calcWindowBounds(state);
	switch (state->mode) {
	case FIND:
		sendFindKeys(state, c);
		break;
	case NORMAL:
		sendNormalKeys(state, c);
		break;
	case INSERT:
		sendTypingKeys(state, c);
		break;
	case VISUAL:
		sendVisualKeys(state, c, false);
		break;
	case COMMAND:
		sendCommandLineKeys(state, c);
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
	case DIFF:
		sendDiffKeys(state, c);
		break;
	case NAMING:
		state->status = "Error: in NAMING mode outside of naming loop";
		break;
	default:
		state->status = std::string("Error: in unknown mode") + getMode(state->mode);
		break;
	}
}
