#include "keybinds/sendKeys.h"
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include "util/cleanup.h"
#include <cstdint>
#include <iostream>

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
			std::cerr << "no file specified when running as root" << std::endl;
			exit(1);
		}
		generateFindFileOutput(state);
	}
	initTerminal();
	calcWindowBounds();
	centerFileExplorer(state);
	renderScreen(state);
	while (true) {
		c = getch();
		if (c != ERR) {
			sendKeys(state, c);
			cleanup(state, c);
			renderScreen(state);
		}
	}
	endwin();
	return 0;
}
