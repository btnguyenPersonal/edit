#include "switchMode.h"

void switchPrevMode(State *state)
{
	auto temp = state->prevMode;
	state->prevMode = state->mode;
	state->mode = temp;
}

void switchMode(State *state, Mode mode)
{
	state->prevMode = state->mode;
	state->mode = mode;
}

