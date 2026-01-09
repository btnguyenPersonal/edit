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
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

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
		if (isWindowPositionInvalid(state)) {
			centerScreen(state);
		}
		assert(state->file->windowPosition.row <= state->file->row);
		assert(state->file->windowPosition.row + state->maxY >= state->file->row);
		if (state->recording.on && !state->dontRecordKey) {
			recordMacroCommand(state, c);
		}
		if (!state->file->jumplist.touched) {
			recordJumpList(state);
		}
		state->matching = matchIt(state);
		realignHarpoon(state);
	}
	state->dontRecordKey = false;

	end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	state->status = std::to_string(elapsed.count());
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
