#include "repeat.h"
#include "keys.h"

void setDotCommand(State *state, int32_t c)
{
	if (!state->dontRecordKey) {
		state->dotCommand.clear();
		state->dotCommand.push_back(getEscapedChar(c));
	}
}

void setDotCommand(State *state, std::vector<int32_t> s)
{
	if (!state->dontRecordKey) {
		state->dotCommand.clear();
		for (uint32_t i = 0; i < s.size(); i++) {
			state->dotCommand.push_back(getEscapedChar(s[i]));
		}
	}
}
