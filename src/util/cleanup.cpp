#include "cleanup.h"
#include "assert.h"
#include "ctrl.h"
#include "display.h"
#include "harpoon.h"
#include "history.h"
#include "movement.h"
#include "sanity.h"
#include "save.h"
#include "textedit.h"
#include "visual.h"
#include <filesystem>
#include <ncurses.h>

int32_t cleanKey(int32_t c) {
	if (c == KEY_BACKSPACE || c == 127) {
		return ctrl('h');
	}
	return c;
}

void cleanup(State *state) {
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
		if (!state->file->jumplist.touched) {
			recordJumpList(state);
		}
		realignHarpoon(state);
	}
	state->dontRecordKey = false;
}

void cleanup(State *state, char c) {
	if (state->recording.on && !state->dontRecordKey) {
		recordMacroCommand(state, c);
	}
	cleanup(state);
}

void history(State *state) {
	if (state->mode == NORMAL) {
		if (!state->dontComputeHistory) {
			std::vector<diffLine> diff = generateDiff(state->file->previousState, state->file->data);
			if (diff.size() != 0) {
				recordHistory(state, diff);
			}
		}
		state->file->previousState = state->file->data;
	}
}

void preRenderCleanup(State *state) {
	if (state->file) {
		state->matching = matchIt(state);
		if (isWindowPositionInvalid(state)) {
			centerScreen(state);
		}
		assert(state->file->windowPosition.row <= state->file->row);
		assert(state->file->windowPosition.row + state->maxY >= state->file->row);
	}
}

void autosaveFile(State *state) {
	if (state->file) {
		if (state->options.autosave && !state->runningAsRoot && !state->file->newFile && !state->dontRecordKey && !isLargeFile(state)) {
			saveFile(state);
		}
	}
}

void autoloadFile(State *state) {
	if (state->file && state->options.autoload && !state->file->newFile) {
		try {
			auto currentModified = std::filesystem::last_write_time(state->file->filename);
			if (currentModified > state->file->lastModified) {
				state->reloadFile(state->file->filename);
			}
			state->shouldNotReRender.clear();
		} catch (...) {
		}
	}
}
