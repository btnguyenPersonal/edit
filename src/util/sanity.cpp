#include "sanity.h"

void sanityCheckGrepSelection(State *state)
{
	if (state->mode == GREP) {
		sanityCheckQuery(state->grep, state->grepOutput.size());
	}
}

void sanityCheckFindFileSelection(State *state)
{
	if (state->mode == FINDFILE) {
		sanityCheckQuery(state->find, state->findOutput.size());
	}
}

void sanityCheckQuery(Query &query, uint32_t len)
{
	if (query.selection >= len) {
		if (len > 0) {
			query.selection = len - 1;
		} else {
			query.selection = 0;
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
