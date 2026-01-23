#include "sanity.h"
#include "textbuffer.h"

void resetValidCursorState(State *state)
{
	if (state->file->data[state->file->row].length() <= state->file->col) {
		if (state->file->data[state->file->row].length() != 0) {
			state->file->col = state->file->data[state->file->row].length() - 1;
		} else {
			state->file->col = 0;
		}
	}
}

void sanityCheckExplorer(State *state)
{
	if (state->explorer.windowLine < 0) {
		state->explorer.windowLine = 0;
	}
	if (state->explorer.index < 0) {
		state->explorer.index = 0;
	} else if (state->explorer.index >= state->explorer.root->getTotalChildren()) {
		state->explorer.index = state->explorer.root->getTotalChildren() - 1;
	}
}

void sanityCheckGrepSelection(State *state)
{
	state->grepMutex.lock();
	sanityCheckQuery(state->grep, state->grepOutput.size());
	state->grepMutex.unlock();
}

void sanityCheckFindSelection(State *state)
{
	state->findMutex.lock();
	sanityCheckQuery(state->find, state->findOutput.size());
	state->findMutex.unlock();
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
		if (textbuffer_getLineCount(state) == 0) {
			textbuffer_insertLine(state, 0, "");
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
		if (textbuffer_getLineCount(state) == 0) {
			state->file->row = 0;
		} else if (state->file->row >= textbuffer_getLineCount(state)) {
			state->file->row = textbuffer_getLineCount(state) - 1;
		}
	}
}
