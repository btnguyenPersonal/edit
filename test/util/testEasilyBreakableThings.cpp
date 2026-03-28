#include "testEasilyBreakableThings.h"
#include "debug.h"

void play(State *state, int32_t c) {
	ungetch(c);
	computeFrame(state);
}

struct testSuiteRun testEasilyBreakableThings() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {""});
		initTerminal();
		play(state, 'q');
		play(state, 'w');
		play(state, 'a');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, ctrl('h'));
		play(state, KEY_BACKSPACE);
		play(state, KEY_BACKSPACE);
		play(state, KEY_BACKSPACE);
		play(state, 127);
		play(state, 127);
		play(state, 127);
		play(state, KEY_ESCAPE);
		play(state, 'q');
		play(state, '@');
		play(state, 'w');
		play(state, '@');
		play(state, 'w');
		play(state, '@');
		play(state, 'w');
		play(state, '@');
		play(state, 'w');
		play(state, '@');
		play(state, 'w');
		endwin();
		output.push_back({"backspace and 127 and ctrl('h') work with macro", compare(state->file->data, {"bbbbbb"})});
	}

	{
		State *state = new State("./test-file.h", {""});
		initTerminal();
		play(state, 'a');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, 'b');
		play(state, ctrl('h'));
		play(state, KEY_BACKSPACE);
		play(state, KEY_BACKSPACE);
		play(state, KEY_BACKSPACE);
		play(state, 127);
		play(state, 127);
		play(state, 127);
		play(state, KEY_ESCAPE);
		play(state, 'q');
		play(state, '.');
		play(state, '.');
		play(state, '.');
		play(state, '.');
		play(state, '.');
		play(state, '.');
		endwin();
		output.push_back({"backspace and 127 and ctrl('h') work with dot", compare(state->file->data, {"bbbbbb"})});
	}

	{
		State *state = new State("./test-file.h", {""});
		initTerminal();
		play(state, 'V');
		play(state, ':');
		play(state, KEY_ESCAPE);
		endwin();
		output.push_back({"escape puts back to prev mode", compare(state->mode, VISUAL)});
	}

	return {"test/util/testEasilyBreakableThings.cpp", output};
}
