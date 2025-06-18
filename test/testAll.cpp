#include <iostream>
#include <stdlib.h>
#include "../src/keybinds/sendKeys.h"
#include "../src/util/state.h"
#include "../src/util/keys.h"
#include "../src/util/cleanup.h"

void send(State *state, std::string ch)
{
	char c = getUnEscapedChar(ch);
	sendKeys(state, c);
	cleanup(state, c);
}

State * beforeEach() {
	system("cp example-file.h test-file.h");
	return new State("test-file.h");
}

void afterEach(State *state, std::vector<std::string> keys) {
	std::string filename = "";
	for (uint32_t i = 0; i < keys.size(); i++) {
		filename += keys[i];
	}
	state->print(std::string("snapshots/") + filename, false, false);
	delete state;
}

int32_t snapshotTest(std::vector<std::string> keys) {
	auto state = beforeEach();
	for (uint32_t i = 0; i < keys.size(); i++) {
		send(state, keys[i]);
	}
	afterEach(state, keys);
	return 0;
}

int main()
{
	snapshotTest({"d", "d"});
	snapshotTest({"G", "V", "g", "g", "d", "P"});
	snapshotTest({":", "4", "0", "<C-J>", "d", "d", "d", "d", "c", "c", "<Esc>", "u", "u", "u"});
	snapshotTest({":", "4", "5", "<C-J>", "d", "j", "k", "k", "x", "u", "u", "u"});
	snapshotTest({":", "4", "5", "<C-J>", "d", "j", "k", "w", "d", "i", "w", "u", "u", "u"});
	snapshotTest({
		"J", "J", "J", "J", "J", "J", "J", "J", "J", "J", "J", "J", "J", "J", "J", "J",
		"u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u"
	});
}
