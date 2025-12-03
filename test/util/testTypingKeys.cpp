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
		output.push_back({ "sendTypingKeys should set mode to SHORTCUTS when pressing escape", compare(state->mode, SHORTCUTS) });
		output.push_back({ "sendTypingKeys should set mode to SHORTCUTS when pressing escape", compare(state->mode, TYPING) });
	}

	return { "test/util/testTypingKeys.cpp", output };
}
