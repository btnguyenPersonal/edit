#include "testMovement.h"
#include "../../src/util/movement.h"

struct testSuiteRun testMovement()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		left(state);
		output.push_back({ "left 0", compare(state->file->col, 0) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		state->file->col = 5000;
		left(state);
		output.push_back({ "left 1", compare(state->file->col, 0) });
	}


	{
		State *state = new State("./test-file.h", { "aaaaaaaa" });
		state->file->col = 5;
		left(state);
		output.push_back({ "left 2", compare(state->file->col, 4) });
	}

	{
		State *state = new State("./test-file.h", { "aaaa" });
		state->file->col = 0;
		right(state);
		output.push_back({ "right 0", compare(state->file->col, 1) });
		right(state);
		output.push_back({ "right 1", compare(state->file->col, 2) });
		right(state);
		output.push_back({ "right 2", compare(state->file->col, 3) });
		right(state);
		output.push_back({ "right 3", compare(state->file->col, 4) });
		state->file->col = 5000;
		right(state);
		output.push_back({ "right 4", compare(state->file->col, 4) });
		state->file->row = 5000;
		right(state);
		output.push_back({ "right 5", compare(state->file->col, 4) });
	}

	return { "test/util/testMovement.cpp", output };
}

