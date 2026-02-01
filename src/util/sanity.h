#pragma once

#include "state.h"
#include "query.h"

void sanityCheckExplorer(State *state);
void resetValidCursorState(State *state);
void sanityCheckQuery(Query &query, uint32_t len);
void fixColOverMax(State *state);
void sanityCheckRowOutOfBounds(State *state);
void sanityCheckGrepSelection(State *state);
void sanityCheckFindSelection(State *state);
void sanityCheckDocumentEmpty(State *state);
