#pragma once

#include "state.h"

void fixColOverMax(State *state);
void sanityCheckRowOutOfBounds(State *state);
void sanityCheckGrepSelection(State *state);
void sanityCheckFindFileSelection(State *state);
void sanityCheckRowColOutOfBounds(State *state);
void sanityCheckDocumentEmpty(State *state);
