#include "harpoon.h"

void moveHarpoonRight(State *state)
{
	if (state->harpoon[state->workspace].index + 1 < state->harpoon[state->workspace].list.size()) {
		auto temp = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index] = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index + 1];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index + 1] = temp;
		state->harpoon[state->workspace].index += 1;
	}
}

void moveHarpoonLeft(State *state)
{
	if (state->harpoon[state->workspace].index > 0 && state->harpoon[state->workspace].index < state->harpoon[state->workspace].list.size()) {
		auto temp = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index] = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index - 1];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index - 1] = temp;
		state->harpoon[state->workspace].index -= 1;
	}
}

bool eraseHarpoon(State *state)
{
	if (state->harpoon[state->workspace].index < state->harpoon[state->workspace].list.size()) {
		state->harpoon[state->workspace].list.erase(state->harpoon[state->workspace].list.begin() + state->harpoon[state->workspace].index);
		return true;
	}
	return false;
}

void clearHarpoon(State *state)
{
	state->harpoon[state->workspace].index = 0;
	state->harpoon[state->workspace].list.clear();
}

void jumpToPrevHarpoon(State *state)
{
	if (state->harpoon[state->workspace].index > 0) {
		jumpToHarpoon(state, state->harpoon[state->workspace].index - 1);
	} else {
		focusHarpoon(state);
	}
}

void jumpToNextHarpoon(State *state)
{
	if (state->harpoon[state->workspace].index + 1 < state->harpoon[state->workspace].list.size()) {
		jumpToHarpoon(state, state->harpoon[state->workspace].index + 1);
	} else {
		focusHarpoon(state);
	}
}

void focusHarpoon(State *state)
{
	jumpToHarpoon(state, state->harpoon[state->workspace].index);
}

void realignHarpoon(State *state)
{
	for (uint32_t i = 0; i < state->harpoon[state->workspace].list.size(); i++) {
		if (state->harpoon[state->workspace].list[i] == state->file->filename) {
			state->harpoon[state->workspace].index = i;
		}
	}
}

bool containsHarpoon(State *state)
{
	for (uint32_t i = 0; i < state->harpoon[state->workspace].list.size(); i++) {
		if (state->file->filename == state->harpoon[state->workspace].list[i]) {
			return true;
		}
	}
	return false;
}

bool createNewestHarpoon(State *state)
{
	if (containsHarpoon(state)) {
		return false;
	}
	if (state->workspace >= state->harpoon.size()) {
		state->status = "harpoon workspace over limit";
		return false;
	}
	state->harpoon[state->workspace].list.push_back(state->file->filename);
	state->harpoon[state->workspace].index = state->harpoon[state->workspace].list.size() - 1;
	return true;
}

bool jumpToHarpoon(State *state, uint32_t num)
{
	if (num < state->harpoon[state->workspace].list.size()) {
		if (!state->resetState(state->harpoon[state->workspace].list[num])) {
			state->harpoon[state->workspace].list.erase(state->harpoon[state->workspace].list.begin() + num);
			return false;
		}
		state->harpoon[state->workspace].index = num;
		return true;
	}
	return false;
}

