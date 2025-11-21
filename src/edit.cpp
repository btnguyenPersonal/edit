#include "keybinds/sendKeys.h"
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include "util/cleanup.h"
#include "util/find.h"
#include <cstdint>
#include <iostream>
#include <chrono>

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
		generateFindOutput(state);
	}
	initTerminal();
	calcWindowBounds();
	centerFileExplorer(state);
	renderScreen(state);
	std::chrono::_V2::system_clock::time_point start;
	std::chrono::_V2::system_clock::time_point stop;
	while (true) {
		c = getch();
		if (c != ERR) {
			start = std::chrono::high_resolution_clock::now();
			sendKeys(state, c);
			stop = std::chrono::high_resolution_clock::now();
			auto keysDuration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			start = std::chrono::high_resolution_clock::now();
			cleanup(state, c);
			stop = std::chrono::high_resolution_clock::now();
			auto cleanupDuration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			start = std::chrono::high_resolution_clock::now();
			renderScreen(state);
			stop = std::chrono::high_resolution_clock::now();
			auto renderDuration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			state->status = std::string("keys: ") + std::to_string(keysDuration.count()) + std::string(" cleanup: ") + std::to_string(cleanupDuration.count()) + std::string(" render: ") + std::to_string(renderDuration.count());
			renderScreen(state);
		}
	}
	endwin();
	return 0;
}
