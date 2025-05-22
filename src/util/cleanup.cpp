#include "helper.h"
#include "expect.h"
#include "history.h"
#include "cleanup.h"
#include "expect.h"
#include <ncurses.h>

void cleanup(State *state, char c)
{
	if (state->mode == SHORTCUTS && state->filename == "") {
		endwin();
		exit(0);
	} else if (state->filename != "") {
		sanityCheckDocumentEmpty(state);
		sanityCheckRowColOutOfBounds(state);
		expect(state->data[state->row].length() >= 0);
		if (state->options.insert_final_newline) {
			insertFinalEmptyNewline(state);
		}
		if (isWindowPositionInvalid(state)) {
			centerScreen(state);
		}
		expect(state->windowPosition.row <= state->row);
		expect(state->windowPosition.row + state->maxY > state->row);
		if (state->recording && state->dontRecordKey == false) {
			if (!(c == ',' && state->mode == SHORTCUTS)) {
				recordMacroCommand(state, c);
			}
		}
		if (c != ctrl('z') && c != ctrl('q')) {
			recordJumpList(state);
		}
		if (!state->recording && state->mode == SHORTCUTS) {
			std::vector<diffLine> diff = generateDiff(state->previousState, state->data);
			if (diff.size() != 0) {
				if (state->options.autosave && !state->unsavedFile) {
					saveFile(state);
				}
				state->previousState = state->data;
				if (c != ctrl('r') && c != 'u') {
					recordHistory(state, diff);
				}
			}
		}
		state->matching = matchIt(state);
		focusHarpoon(state);
	}
	state->dontRecordKey = false;
}
