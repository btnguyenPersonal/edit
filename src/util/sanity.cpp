#include "sanity.h"

void sanityCheckGrepSelection(State *state)
{
	if (state->mode == GREP && state->grep.selection >= state->grepOutput.size()) {
		if (state->grepOutput.size() > 0) {
			state->grep.selection = state->grepOutput.size() - 1;
		} else {
			state->grep.selection = 0;
		}
	}
}

void sanityCheckFindFileSelection(State *state)
{
	if (state->mode == FINDFILE && state->find.selection >= state->findOutput.size()) {
		if (state->findOutput.size() > 0) {
			state->find.selection = state->findOutput.size() - 1;
		} else {
			state->find.selection = 0;
		}
	}
}

void sanityCheckDocumentEmpty(State *state)
{
	if (state->data.size() == 0) {
		state->data.push_back("");
	}
}

void fixColOverMax(State *state)
{
	sanityCheckDocumentEmpty(state);
	sanityCheckRowOutOfBounds(state);
	if (state->col > state->data[state->row].length()) {
		state->col = state->data[state->row].length();
	}
}

void sanityCheckRowOutOfBounds(State *state)
{
	if (state->data.size() == 0) {
		state->row = 0;
	} else if (state->row >= state->data.size()) {
		state->row = state->data.size() - 1;
	}
}

void sanityCheckRowColOutOfBounds(State *state)
{
	sanityCheckRowOutOfBounds(state);
	if (state->mode == TYPING) {
		fixColOverMax(state);
	}
}
