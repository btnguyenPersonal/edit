#include "sanity.h"

void sanityCheckGrepSelection(State *state)
{
	sanityCheckQuery(state->grep, state->grepOutput.size());
}

void sanityCheckFindSelection(State *state)
{
	sanityCheckQuery(state->find, state->findOutput.size());
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
	if (state->file) {
		if (state->file->data.size() == 0) {
			state->file->data.push_back("");
		}
	}
}

void fixColOverMax(State *state)
{
	if (state->file) {
		sanityCheckDocumentEmpty(state);
		sanityCheckRowOutOfBounds(state);
		if (state->file->col > state->file->data[state->file->row].length()) {
			state->file->col = state->file->data[state->file->row].length();
		}
	}
}

void sanityCheckRowOutOfBounds(State *state)
{
	if (state->file) {
		if (state->file->data.size() == 0) {
			state->file->row = 0;
		} else if (state->file->row >= state->file->data.size()) {
			state->file->row = state->file->data.size() - 1;
		}
	}
}
