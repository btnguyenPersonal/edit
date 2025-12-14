#include "helper.h"
#include "expect.h"
#include "history.h"
#include "cleanup.h"
#include "expect.h"
#include "save.h"
#include "sanity.h"
#include <ncurses.h>

void cleanup(State *state, char c)
{
	if (state->mode == NORMAL && state->filename == "") {
		endwin();
		exit(0);
	} else if (state->filename != "") {
		if (!state->skipSetHardCol) {
			state->hardCol = getDisplayCol(state);
		}
		state->skipSetHardCol = false;
		sanityCheckGrepSelection(state);
		sanityCheckFindSelection(state);
		sanityCheckDocumentEmpty(state);
		sanityCheckRowOutOfBounds(state);
		if (state->mode == INSERT) {
			fixColOverMax(state);
		}
		expect(state->data[state->row].length() >= 0);
		if (state->options.insert_final_newline) {
			insertFinalEmptyNewline(state);
		}
		if (isWindowPositionInvalid(state)) {
			centerScreen(state);
		}
		expect(state->windowPosition.row <= state->row);
		expect(state->windowPosition.row + state->maxY > state->row);
		if (state->recording && !state->dontRecordKey) {
			if (!(c == ',' && state->mode == NORMAL)) {
				recordMacroCommand(state, c);
			}
		}
		if (!state->jumplist.touched) {
			recordJumpList(state);
		}
		state->matching = matchIt(state);
		realignHarpoon(state);
		if (!isLargeFile(state) && state->mode == NORMAL && state->options.autosave && !state->runningAsRoot && !state->dontRecordKey) {
			saveFile(state);
		}
	}
	state->dontRecordKey = false;
}

void history(State *state, char c)
{
	if (state->mode == NORMAL) {
		std::vector<diffLine> diff = generateDiff(state->previousState, state->data);
		state->previousState = state->data;
		if (diff.size() != 0) {
			if (c != ctrl('r') && c != 'u') {
				recordHistory(state, diff);
			}
		}
	}
}
