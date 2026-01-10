#include "assert.h"
#include "history.h"
#include "cleanup.h"
#include "save.h"
#include "sanity.h"
#include "visual.h"
#include "display.h"
#include "textedit.h"
#include "harpoon.h"
#include "movement.h"
#include "ctrl.h"
#include <ncurses.h>

void cleanup(State *state, char c)
{
	if (state->mode == NORMAL && !state->file) {
		endwin();
		exit(0);
	} else if (state->file) {
		sanityCheckGrepSelection(state);
		sanityCheckFindSelection(state);
		sanityCheckDocumentEmpty(state);
		assert(state->file->data.size() > 0);
		sanityCheckRowOutOfBounds(state);
		assert(state->file->row < state->file->data.size());
		if (!state->skipSetHardCol) {
			state->file->hardCol = getDisplayCol(state);
		}
		state->skipSetHardCol = false;
		if (state->mode == INSERT) {
			fixColOverMax(state);
		}
		if (state->options.insert_final_newline) {
			insertFinalEmptyNewline(state);
		}
		if (state->recording.on && !state->dontRecordKey) {
			recordMacroCommand(state, c);
		}
		if (!state->file->jumplist.touched) {
			recordJumpList(state);
		}
		realignHarpoon(state);
	}
	state->dontRecordKey = false;
}

void history(State *state, char c)
{
	if (state->mode == NORMAL) {
		std::vector<diffLine> diff = generateDiff(state->file->previousState, state->file->data);
		state->file->previousState = state->file->data;
		if (diff.size() != 0) {
			if (c != ctrl('r') && c != 'u') {
				recordHistory(state, diff);
			}
		}
	}
}

void preRenderCleanup(State *state)
{
	if (state->file) {
		state->matching = matchIt(state);
		if (isWindowPositionInvalid(state)) {
			centerScreen(state);
		}
		assert(state->file->windowPosition.row <= state->file->row);
		assert(state->file->windowPosition.row + state->maxY >= state->file->row);
	}
}

void autosaveFile(State* state)
{
	if (state->file) {
		if (state->options.autosave && !state->runningAsRoot && !state->dontRecordKey && !isLargeFile(state)) {
			saveFile(state);
		}
	}
}
