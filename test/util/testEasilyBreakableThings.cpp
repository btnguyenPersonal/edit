#include "testEasilyBreakableThings.h"

struct testSuiteRun testEasilyBreakableThings()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		initTerminal();
		system("sleep 2");
		endwin();
	}

	return { "test/util/testEasilyBreakableThings.cpp", output };
}

