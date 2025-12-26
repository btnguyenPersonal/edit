#pragma once

#include "state.h"

void moveHarpoonLeft(State *state);
void moveHarpoonRight(State *state);
void focusHarpoon(State *state);
bool eraseHarpoon(State *state);
bool containsHarpoon(State *state);
void clearHarpoon(State *state);
bool createNewestHarpoon(State *state);
void jumpToPrevHarpoon(State *state);
void jumpToNextHarpoon(State *state);
bool jumpToHarpoon(State *state, uint32_t num);
void realignHarpoon(State *state);
