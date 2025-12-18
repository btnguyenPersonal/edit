#include "helper.h"
#include "assert.h"
#include "history.h"
#include "cleanup.h"
#include "save.h"
#include "sanity.h"
#include <ncurses.h>

void cleanup(State *state, char c)
{
	if (state->mode == NORMAL && !state->file) {
		endwin();
		exit(0);
	} else if (state->file) {
		if (!state->skipSetHardCol) {
			state->file->hardCol = getDisplayCol(state);
		}
		state->skipSetHardCol = false;
		sanityCheckGrepSelection(state);
		sanityCheckFindSelection(state);
		sanityCheckDocumentEmpty(state);
		sanityCheckRowOutOfBounds(state);
		if (state->mode == INSERT) {
			fixColOverMax(state);
		}
		assert(state->file->data[state->file->row].length() >= 0);
		if (state->options.insert_final_newline) {
			insertFinalEmptyNewline(state);
		}
		if (isWindowPositionInvalid(state)) {
			centerScreen(state);
		}
		assert(state->file->windowPosition.row <= state->file->row);
		assert(state->file->windowPosition.row + state->maxY > state->file->row);
		if (state->recording && !state->dontRecordKey) {
			if (!(c == ',' && state->mode == NORMAL)) {
				recordMacroCommand(state, c);
			}
		}
		if (!state->file->jumplist.touched) {
			recordJumpList(state);
		}
		state->matching = matchIt(state);
		realignHarpoon(state);
	}
	state->dontRecordKey = false;
}

void history(State *state, char c)
{
	if (!isLargeFile(state) && state->mode == NORMAL) {
		std::vector<diffLine> diff = generateDiff(state->file->previousState, state->file->data);
		state->file->previousState = state->file->data;
		if (diff.size() != 0) {
			if (c != ctrl('r') && c != 'u') {
				recordHistory(state, diff);
			}
			if (state->options.autosave && !state->runningAsRoot && !state->dontRecordKey) {
				saveFile(state);
			}
		}
	}
}
