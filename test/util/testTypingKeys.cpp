#include "../../src/keybinds/sendTypingKeys.h"
#include "../../src/util/state.h"
#include "../../src/util/compare.h"
#include "../../src/util/ctrl.h"
#include "../test.h"
#include <ncurses.h>

struct testSuiteRun testTypingKeys()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		sendTypingKeys(state, 'c');
		output.push_back({ "sendTypingKeys should add a character on an empty line", compare(state->data, { "c" }) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		sendTypingKeys(state, 27);
		output.push_back({ "sendTypingKeys <Esc> should set mode to SHORTCUTS", compare(state->mode, SHORTCUTS) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		sendTypingKeys(state, 'a');
		sendTypingKeys(state, 'b');
		sendTypingKeys(state, 'c');
		sendTypingKeys(state, 27);
		std::vector<std::string> expect = { "a", "b", "c", "<Esc>" };
		output.push_back({ "sendTypingKeys <Esc> should set dotCommand to abcESC when abcESC is input", compare(state->dotCommand, expect) });
		output.push_back({ "sendTypingKeys <Esc> should move the cursor left one", compare(state->col, 2) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World" });
		state->col = 1000;
		sendTypingKeys(state, '!');
		output.push_back({ "sendTypingKeys should insert at the very end of the line if the col is too big", compare(state->data, { "Hello World!" }) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 4;
		state->col = 5;
		sendTypingKeys(state, '!');
		output.push_back({ "sendTypingKeys should insert on the correct row and column", compare(state->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello! World", "Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 4;
		state->col = 5;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline in the middle of a line", compare(state->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello", " World", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->col, 0) });
		output.push_back({ "  and row is 5", compare(state->row, 5) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 4;
		state->col = 0;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the start of a line", compare(state->data, { "Hello World", "Hello World", "Hello World", "Hello World", "", "Hello World", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->col, 0) });
		output.push_back({ "  and row is 5", compare(state->row, 5) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 4;
		state->col = 1000;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the end of a line", compare(state->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->col, 0) });
		output.push_back({ "  and row is 5", compare(state->row, 5) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 0;
		state->col = 0;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the start of a file", compare(state->data, { "", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->col, 0) });
		output.push_back({ "  and row is 1", compare(state->row, 1) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 50;
		state->col = 1000;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the end of a file", compare(state->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "" }) });
		output.push_back({ "  and col is 0", compare(state->col, 0) });
		output.push_back({ "  and row is 6", compare(state->row, 6) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->row = 50;
		state->col = 1000;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the end of a file", compare(state->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "" }) });
		output.push_back({ "  and col is 0", compare(state->col, 0) });
		output.push_back({ "  and row is 6", compare(state->row, 6) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 1;
		state->col = 1;
		sendTypingKeys(state, KEY_LEFT);
		output.push_back({ "sendTypingKeys KEY_LEFT should move left one", compare(state->col, 0) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 1;
		state->col = 1;
		sendTypingKeys(state, KEY_RIGHT);
		output.push_back({ "sendTypingKeys KEY_RIGHT should move right one", compare(state->col, 2) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 1;
		state->col = 1;
		sendTypingKeys(state, KEY_UP);
		output.push_back({ "sendTypingKeys KEY_UP should move up one", compare(state->row, 0) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 1;
		state->col = 1;
		sendTypingKeys(state, KEY_DOWN);
		output.push_back({ "sendTypingKeys KEY_DOWN should move down one", compare(state->row, 2) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 1;
		state->col = 1;
		sendTypingKeys(state, ctrl('v'));
		sendTypingKeys(state, '\n');
		output.push_back({ "sendTypingKeys ctrl(v) \\n should output a newline character", compare(state->data, { "    Hello World",  " ", "   Hello World", "    Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		for (uint32_t i = 0; i < 1000; i++) {
			sendTypingKeys(state, KEY_BACKSPACE);
		}
		output.push_back({ "sendTypingKeys KEY_BACKSPACE should delete everything if starting at the end", compare(state->data, {""}) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		for (uint32_t i = 0; i < 1000; i++) {
			sendTypingKeys(state, 127);
		}
		output.push_back({ "sendTypingKeys 127 should delete everything if starting at the end", compare(state->data, {""}) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 0;
		sendTypingKeys(state, KEY_BACKSPACE);
		output.push_back({ "sendTypingKeys KEY_BACKSPACE should delete newlines", compare(state->data, { "    Hello World",  "    Hello World    Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		sendTypingKeys(state, KEY_BACKSPACE);
		output.push_back({ "sendTypingKeys KEY_BACKSPACE should delete normal characters", compare(state->data, { "    Hello World",  "    Hello World", "    Hello Worl" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 0;
		sendTypingKeys(state, ctrl('t'));
		output.push_back({ "sendTypingKeys ctrl(t) should insert a tab", compare(state->data, { "    Hello World",  "    Hello World", "	    Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		insertNewline(state);
		sendTypingKeys(state, 'a');
		output.push_back({ "sendTypingKeys typing at the start of a newline should indent", compare(state->data, { "    Hello World",  "    Hello World", "    Hello World", "    a" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should delete whole words", compare(state->data, { "    Hello World",  "    Hello World", "    Hello " }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should delete spaces between words", compare(state->data, { "    Hello World",  "    Hello World", "    " }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World",  "    Hello World", "    Hello World" });
		state->row = 2;
		state->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should delete to the start of line if all spaces", compare(state->data, { "    Hello World",  "    Hello World", "" }) });
	}

	return { "test/util/testTypingKeys.cpp", output };
}
