#include "debug.h"
#include "../../src/util/render.h"

void debug(State *state)
{
	initTerminal();
	renderScreen(state);
	system("sleep 2");
	endwin();
}
