#include "testRender.h"

struct testSuiteRun testRender()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		output.push_back({ "testRender", compare("hi", "hi") });
	}

	return { "test/util/testRender.cpp", output };
}
