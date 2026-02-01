#include "testTypingKeys.h"

struct testSuiteRun testTypingKeys()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		sendTypingKeys(state, 'c');
		output.push_back({ "sendTypingKeys should add a character on an empty line", compare(state->file->data, { "c" }) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		sendTypingKeys(state, 27);
		output.push_back({ "sendTypingKeys <Esc> should set mode to NORMAL", compare(state->mode, NORMAL) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		sendTypingKeys(state, 'a');
		sendTypingKeys(state, 'b');
		sendTypingKeys(state, 'c');
		sendTypingKeys(state, 27);
		std::vector<std::string> expect = { "a", "b", "c", "<Esc>" };
		output.push_back({ "sendTypingKeys <Esc> should set dotCommand to abcESC when abcESC is input", compare(state->dotCommand, expect) });
		output.push_back({ "sendTypingKeys <Esc> should move the cursor left one", compare(state->file->col, 2) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World" });
		state->file->col = 1000;
		sendTypingKeys(state, '!');
		output.push_back({ "sendTypingKeys should insert at the very end of the line if the col is too big", compare(state->file->data, { "Hello World!" }) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 4;
		state->file->col = 5;
		sendTypingKeys(state, '!');
		output.push_back({ "sendTypingKeys should insert on the correct row and column", compare(state->file->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello! World", "Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 4;
		state->file->col = 5;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline in the middle of a line", compare(state->file->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello", " World", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->file->col, 0) });
		output.push_back({ "  and row is 5", compare(state->file->row, 5) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 4;
		state->file->col = 0;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the start of a line", compare(state->file->data, { "Hello World", "Hello World", "Hello World", "Hello World", "", "Hello World", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->file->col, 0) });
		output.push_back({ "  and row is 5", compare(state->file->row, 5) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 4;
		state->file->col = 1000;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the end of a line", compare(state->file->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->file->col, 0) });
		output.push_back({ "  and row is 5", compare(state->file->row, 5) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 0;
		state->file->col = 0;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the start of a file", compare(state->file->data, { "", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" }) });
		output.push_back({ "  and col is 0", compare(state->file->col, 0) });
		output.push_back({ "  and row is 1", compare(state->file->row, 1) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 50;
		state->file->col = 1000;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the end of a file", compare(state->file->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "" }) });
		output.push_back({ "  and col is 0", compare(state->file->col, 0) });
		output.push_back({ "  and row is 6", compare(state->file->row, 6) });
	}

	{
		State *state = new State("./test-file.h", { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World" });
		state->file->row = 50;
		state->file->col = 1000;
		insertNewline(state);
		output.push_back({ "insertNewline should insert a newline at the end of a file", compare(state->file->data, { "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "Hello World", "" }) });
		output.push_back({ "  and col is 0", compare(state->file->col, 0) });
		output.push_back({ "  and row is 6", compare(state->file->row, 6) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 1;
		state->file->col = 1;
		sendTypingKeys(state, KEY_LEFT);
		output.push_back({ "sendTypingKeys KEY_LEFT should move left one", compare(state->file->col, 0) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 1;
		state->file->col = 1;
		sendTypingKeys(state, KEY_RIGHT);
		output.push_back({ "sendTypingKeys KEY_RIGHT should move right one", compare(state->file->col, 2) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 1;
		state->file->col = 1;
		sendTypingKeys(state, KEY_UP);
		output.push_back({ "sendTypingKeys KEY_UP should move up one", compare(state->file->row, 0) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 1;
		state->file->col = 1;
		sendTypingKeys(state, KEY_DOWN);
		output.push_back({ "sendTypingKeys KEY_DOWN should move down one", compare(state->file->row, 2) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 1;
		state->file->col = 1;
		sendTypingKeys(state, ctrl('v'));
		sendTypingKeys(state, '\n');
		output.push_back({ "sendTypingKeys ctrl(v) \\n should output a newline character", compare(state->file->data, { "    Hello World", " ", "   Hello World", "    Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		for (uint32_t i = 0; i < 1000; i++) {
			sendTypingKeys(state, ctrl('h'));
		}
		output.push_back({ "sendTypingKeys ctrl('h') should delete everything if starting at the end", compare(state->file->data, { "" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		for (uint32_t i = 0; i < 1000; i++) {
			sendTypingKeys(state, ctrl('h'));
		}
		output.push_back({ "sendTypingKeys ctrl('h') should delete everything if starting at the end", compare(state->file->data, { "" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 0;
		sendTypingKeys(state, ctrl('h'));
		output.push_back({ "sendTypingKeys ctrl('h') should delete newlines", compare(state->file->data, { "    Hello World", "    Hello World    Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		sendTypingKeys(state, ctrl('h'));
		output.push_back({ "sendTypingKeys ctrl('h') should delete normal characters", compare(state->file->data, { "    Hello World", "    Hello World", "    Hello Worl" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 0;
		sendTypingKeys(state, ctrl('t'));
		output.push_back({ "sendTypingKeys ctrl(t) should insert a tab", compare(state->file->data, { "    Hello World", "    Hello World", "	    Hello World" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		insertNewline(state);
		sendTypingKeys(state, 'a');
		output.push_back({ "sendTypingKeys typing at the start of a newline should indent", compare(state->file->data, { "    Hello World", "    Hello World", "    Hello World", "    a" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should delete whole words", compare(state->file->data, { "    Hello World", "    Hello World", "    Hello " }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should delete spaces between words", compare(state->file->data, { "    Hello World", "    Hello World", "    " }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello World" });
		state->file->row = 2;
		state->file->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should delete to the start of line if all spaces", compare(state->file->data, { "    Hello World", "    Hello World", "" }) });
	}

	{
		State *state = new State("./test-file.h", { "    Hello World", "    Hello World", "    Hello Wor(ld" });
		state->file->row = 2;
		state->file->col = 1000;
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		sendTypingKeys(state, ctrl('w'));
		output.push_back({ "sendTypingKeys ctrl(w) should not stop at (", compare(state->file->data, { "    Hello World", "    Hello World", "    Hello " }) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		state->file->row = 0;
		state->file->col = 0;
		for (char c = ' '; c <= '~'; c++) {
			sendTypingKeys(state, c);
		}
		output.push_back({ "sendTypingKeys should print all valid characters", compare(state->file->data, { "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" }) });
	}

	return { "test/util/testTypingKeys.cpp", output };
}
