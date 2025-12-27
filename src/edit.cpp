#include "keybinds/mainLoop.h"
#include "util/render.h"
#include "util/state.h"
#include "util/find.h"
#include "util/sanity.h"
#include "util/display.h"
#include <cstdint>

int32_t main(int32_t argc, char *argv[])
{
	int32_t c;
	State *state;
	std::string filename;
	if (argc >= 2) {
		filename = argv[1];
	}
	if (!filename.empty()) {
		state = new State(filename);
	} else {
		state = new State();
		if (state->runningAsRoot) {
			printf("no file specified when running as root\n");
			exit(1);
		}
		generateFindOutput(state);
	}
	state->dontSave = false;
	initTerminal();
	sanityCheckDocumentEmpty(state);
	calcWindowBounds(state);
	renderScreen(state);
	while (true) {
		c = getch();
		mainLoop(state, c);
	}
	endwin();
	return 0;
}
