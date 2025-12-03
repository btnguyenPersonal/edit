#include "../../src/keybinds/sendTypingKeys.h"
#include "../../src/util/state.h"
#include "../../src/util/compare.h"
#include "../test.h"

struct testSuiteRun testTypingKeys() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {""});
		sendTypingKeys(state, 'c');
		output.push_back({ "sendTypingKeys should add a character on an empty line", compare(state->data, {"c"}) });
	}

	{
		State *state = new State("./test-file.h", {""});
		sendTypingKeys(state, 27);
		output.push_back({ "sendTypingKeys <Esc> should set mode to SHORTCUTS", compare(state->mode, SHORTCUTS) });
	}

	{
		State *state = new State("./test-file.h", {""});
		sendTypingKeys(state, 'a');
		sendTypingKeys(state, 'b');
		sendTypingKeys(state, 'c');
		sendTypingKeys(state, 27);
		std::vector<std::string> expect = {"a", "b", "c", "<Esc>"};
		output.push_back({ "sendTypingKeys <Esc> should set dotCommand to abcESC when abcESC is input", compare(state->dotCommand, expect) });
		output.push_back({ "sendTypingKeys <Esc> should move the cursor left one", compare(state->col, 2) });
	}

	return { "test/util/testTypingKeys.cpp", output };
}
