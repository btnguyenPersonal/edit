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

	{
		State *state = new State("./test-file.h", { "aaaa" });
		up(state);
		output.push_back({ "up 0", compare(state->file->row, 0) });
	}

	{
		State *state = new State("./test-file.h", { "aaaa", "ffff", "ffff" });
		state->file->row = 2;
		up(state);
		output.push_back({ "up 1", compare(state->file->row, 1) });
	}

	{
		std::vector<std::string> largerThanScreen;
		for (uint32_t i = 0; i < 25; i++) {
			largerThanScreen.push_back("abc");
		}
		State *state = new State("./test-file.h", largerThanScreen);
		state->file->windowPosition.row = 1;
		state->file->row = 1;
		up(state);
		output.push_back({ "up 2", compare(state->file->row, 0) });
		output.push_back({ "up 2", compare(state->file->windowPosition.row, 0) });
	}

	{
		std::vector<std::string> largerThanScreen;
		for (uint32_t i = 0; i < 25; i++) {
			largerThanScreen.push_back("abc");
		}
		State *state = new State("./test-file.h", largerThanScreen);
		state->file->windowPosition.row = 1;
		state->file->row = 2;
		up(state);
		output.push_back({ "up 3", compare(state->file->row, 1) });
		output.push_back({ "up 3", compare(state->file->windowPosition.row, 1) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		down(state);
		output.push_back({ "down 0", compare(state->file->row, 0) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		down(state);
		output.push_back({ "down 0", compare(state->file->row, 0) });
		output.push_back({ "down 0", compare(state->skipSetHardCol, false) });
	}

	{
		State *state = new State("./test-file.h", { "", "" });
		down(state);
		output.push_back({ "down 1", compare(state->file->row, 1) });
		output.push_back({ "down 1", compare(state->file->col, 0) });
		output.push_back({ "down 1", compare(state->skipSetHardCol, true) });
	}

	{
		State *state = new State("./test-file.h", { "	ffff", "ffff" });
		state->file->col = 2;
		down(state);
		output.push_back({ "down 2", compare(state->file->row, 1) });
		output.push_back({ "down 2", compare(state->file->col, 2) });
		output.push_back({ "down 2", compare(state->skipSetHardCol, true) });
	}

	{
		std::string veryLongString;
		for (uint32_t i = 0; i < 80 * 5; i++) {
			veryLongString += '.';
		}
		std::vector<std::string> longVector;
		for (uint32_t i = 0; i < 23; i++) {
			longVector.push_back(std::string("line number: ") + std::to_string(i));
		}
		longVector.push_back(veryLongString);
		State *state = new State("./test-file.h", longVector);
		state->file->row = 21;

		down(state);

		output.push_back({ "down 3", compare(state->file->row, 22) });
		output.push_back({ "down 3", compare(state->file->col, 0) });
		output.push_back({ "down 3", compare(state->file->windowPosition.row, 0) });

		down(state);

		output.push_back({ "down 4", compare(state->file->row, 23) });
		output.push_back({ "down 4", compare(state->file->col, 0) });
		output.push_back({ "down 4", compare(state->file->windowPosition.row, 1) });
	}

	{
		std::string veryLongString;
		for (uint32_t i = 0; i < 80 * 5; i++) {
			veryLongString += '.';
		}
		std::vector<std::string> longVector;
		for (uint32_t i = 0; i < 23; i++) {
			longVector.push_back(std::string("line number: ") + std::to_string(i));
		}
		longVector.push_back(veryLongString);
		State *state = new State("./test-file.h", longVector);
		state->file->row = 21;
		state->file->col = 1000;

		down(state);

		output.push_back({ "down 5", compare(state->file->row, 22) });
		output.push_back({ "down 5", compare(state->file->col, 1000) });
		output.push_back({ "down 5", compare(state->file->windowPosition.row, 0) });

		down(state);

		output.push_back({ "down 6", compare(state->file->row, 23) });
		output.push_back({ "down 6", compare(state->file->col, 1000) });
		output.push_back({ "down 6", compare(state->file->windowPosition.row, 1) });
	}

	return { "test/util/testMovement.cpp", output };
}

