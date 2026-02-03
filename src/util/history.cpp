#include "history.h"
#include "assert.h"
#include "keys.h"
#include "state.h"
#include <climits>
#include <ncurses.h>
#include <string>
#include <vector>

uint32_t applyDiff(State *state, std::vector<diffLine> diff, bool reverse) {
	uint32_t min = UINT_MAX;
	if (reverse) {
		for (uint32_t i = 0; i < diff.size(); i++) {
			if (diff[i].lineNum < min) {
				min = diff[i].lineNum;
			}
			if (!diff[i].add) {
				if (diff[i].lineNum < state->file->data.size()) {
					state->file->data.insert(state->file->data.begin() + diff[i].lineNum, diff[i].line);
				} else {
					state->file->data.push_back(diff[i].line);
				}
			} else {
				if (diff[i].lineNum < state->file->data.size()) {
					state->file->data.erase(state->file->data.begin() + diff[i].lineNum);
				}
			}
		}
	} else {
		for (int32_t i = diff.size() - 1; i >= 0; i--) {
			if (diff[i].lineNum < min) {
				min = diff[i].lineNum;
			}
			if (diff[i].add) {
				if (diff[i].lineNum < state->file->data.size()) {
					state->file->data.insert(state->file->data.begin() + diff[i].lineNum, diff[i].line);
				} else {
					state->file->data.push_back(diff[i].line);
				}
			} else {
				if (diff[i].lineNum < state->file->data.size()) {
					state->file->data.erase(state->file->data.begin() + diff[i].lineNum);
				}
			}
		}
	}
	return min;
}

std::vector<diffLine> generateDiff(const std::vector<std::string> &a, const std::vector<std::string> &b) {
	std::vector<diffLine> output;
	uint32_t aIndex = 0;
	uint32_t bIndex = 0;
	while (aIndex < a.size() || bIndex < b.size()) {
		if (bIndex >= b.size() && aIndex < a.size()) {
			output.push_back({aIndex, false, a[aIndex]});
			aIndex++;
		} else if (aIndex >= a.size() && bIndex < b.size()) {
			output.push_back({aIndex, true, b[bIndex]});
			bIndex++;
		} else {
			if (a[aIndex] == b[bIndex]) {
				aIndex++;
				bIndex++;
			} else if (a[aIndex] != b[bIndex]) {
				if (a.size() > b.size()) {
					output.push_back({aIndex, false, a[aIndex]});
					aIndex++;
				} else if (b.size() > a.size()) {
					output.push_back({aIndex, true, b[bIndex]});
					bIndex++;
				} else {
					output.push_back({aIndex, false, a[aIndex]});
					aIndex++;
					output.push_back({aIndex, true, b[bIndex]});
					bIndex++;
				}
			}
		}
	}
	return output;
}

void recordHistory(State *state, std::vector<diffLine> diff) {
	if (state->file->historyPosition < (int32_t)state->file->history.size()) {
		state->file->history.erase(state->file->history.begin() + state->file->historyPosition + 1, state->file->history.end());
	}
	state->file->history.push_back(diff);
	state->file->historyPosition = (int32_t)state->file->history.size() - 1;
	state->diffIndex = state->file->historyPosition;
	assert(state->file->historyPosition >= 0);
}

void recordJumpList(State *state) {
	if (state->file->jumplist.list.size() > 0) {
		auto pos = state->file->jumplist.list.back();
		if (pos.row != state->file->row || pos.col != state->file->col) {
			state->file->jumplist.list.erase(state->file->jumplist.list.begin() + state->file->jumplist.index + 1, state->file->jumplist.list.end());
			state->file->jumplist.list.push_back({state->file->row, state->file->col});
			state->file->jumplist.index = state->file->jumplist.list.size() - 1;
		}
	} else {
		state->file->jumplist.list.push_back({state->file->row, state->file->col});
		state->file->jumplist.index = state->file->jumplist.list.size() - 1;
	}
	state->file->jumplist.touched = false;
}

void recordMacroCommand(State *state, char c) {
	state->macroCommand[state->recording.c].push_back(getEscapedChar(c));
}

void recordMotion(State *state, int32_t c) {
	state->motion.push_back(c);
}

void forwardFileStack(State *state) {
	if (state->fileStack.size() > 0 && state->fileStackIndex < state->fileStack.size()) {
		if (state->fileStackIndex + 1 < state->fileStack.size()) {
			state->fileStackIndex += 1;
		}
		if (std::filesystem::is_regular_file(state->fileStack[state->fileStackIndex].c_str())) {
			state->changeFile(state->fileStack[state->fileStackIndex]);
			state->showFileStack = true;
		} else {
			state->status = "file not found: " + state->fileStack[state->fileStackIndex];
			state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
		}
	}
}

void backwardFileStack(State *state) {
	if (state->fileStack.size() > 0 && state->fileStackIndex < state->fileStack.size()) {
		if (state->fileStackIndex > 0) {
			state->fileStackIndex -= 1;
		}
		if (std::filesystem::is_regular_file(state->fileStack[state->fileStackIndex].c_str())) {
			state->changeFile(state->fileStack[state->fileStackIndex]);
			state->showFileStack = true;
		} else {
			state->status = "file not found: " + state->fileStack[state->fileStackIndex];
			state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
		}
	}
}
