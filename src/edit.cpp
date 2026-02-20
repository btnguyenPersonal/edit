#include "keybinds/sendKeys.h"
#include "util/display.h"
#include "util/find.h"
#include "util/mainLoop.h"
#include "util/movement.h"
#include "util/cleanup.h"
#include "util/render.h"
#include "util/save.h"
#include "util/sanity.h"
#include "util/state.h"
#include "util/string.h"
#include <cstdint>
#include <ncurses.h>

int32_t main(int32_t argc, char *argv[]) {
	State *state;
	std::string filename;
	if (argc >= 2) {
		filename = argv[1];
	}
	std::string rowString;
	if (argc >= 3) {
		rowString = safeSubstring(argv[1], 1);
		filename = argv[2];
	}
	std::string command;
	if (argc >= 4) {
		command = argv[3];
	}
	if (!filename.empty()) {
		state = new State(filename);
		if (!rowString.empty()) {
			try {
				auto row = stoi(rowString);
				if (row > 0) {
					state->file->row = row - 1;
					centerScreen(state);
				}
			} catch (...) {
			}
		}
	} else {
		state = new State();
		if (state->runningAsRoot) {
			printf("no file specified when running as root\n");
			exit(1);
		}
		generateFindOutput(state);
	}
	state->dontSave = false;
	sanityCheckDocumentEmpty(state);
	if (!command.empty()) {
		for (uint32_t i = 0; i < command.length(); i++) {
			sendKeys(state, command[i]);
			cleanup(state, command[i]);
			history(state);
		}
		saveFile(state);
	} else {
		initTerminal();
		calcWindowBounds(state);
		mainLoop(state);
	}
	return 0;
}
