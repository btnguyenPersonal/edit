#include <iostream>
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

int main()
{
	// set file to X code
	// print state to a file
	// compare /tmp/file to /snapshot/file

	// add crash reports
	// std::filesystem::path home = std::filesystem::absolute(getenv("HOME"));
	// state->print(home.string() + "/.crashreport");

	State *state = new State("longtest.js");

	send(state, ":");
	send(state, "1");
	send(state, "2");
	send(state, "<C-J>");
	send(state, "d");
	send(state, "d");
	send(state, "P");
	send(state, "P");
	state->print("log", false, false);
}
