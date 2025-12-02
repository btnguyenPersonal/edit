#include "../../src/util/sanity.h"
#include "../../src/util/state.h"
#include "../../src/util/compare.h"
#include "../test.h"

struct testSuiteRun testSanityChecks() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {});
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should add an empty line if the data is empty", compare(state->data, {""}) });
	}

	{
		State *state = new State("./test-file.h", {""});
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should set col to 0 if col is already 0 and the line is empty", compare(state->col, 0) });
	}

	{
		State *state = new State("./test-file.h", {""});
		state->col = 1;
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should set col to 0 if col is 1 and the line is empty", compare(state->col, 0) });
	}

	{
		State *state = new State("./test-file.h", {""});
		state->row = 1;
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should set row to 0 if row is 1 and the row does not exist", compare(state->row, 0) });
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->col = 1000;
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should set col to the end in the trivial case", compare(state->col, std::string("Hello world!").length()) });
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->col = 13;
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should set col to the end when one after the end", compare(state->col, std::string("Hello world!").length()) });
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->col = 11;
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should not set col to the end when one before the end", compare(state->col, 11) });
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->col = 0;
		fixColOverMax(state);
		output.push_back({ "fixColOverMax should not set col to the end when at the start", compare(state->col, 0) });
	}

	return { "test/util/testSanityChecks.cpp", output };
}
