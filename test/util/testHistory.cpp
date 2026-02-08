#include "testHistory.h"
#include "../../src/keybinds/sendKeys.h"
#include "../../src/util/cleanup.h"
#include "../../src/util/history.h"
#include "../../src/util/ctrl.h"

struct testSuiteRun testHistory() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {"hello"});
		uint32_t initialHistorySize = state->file->history.size();

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		output.push_back({
			"normal command recorded",
			compare((uint32_t)state->file->history.size() == initialHistorySize + 1, true)
		});

		output.push_back({
			"normal command changes data",
			compare(state->file->data[0] == "ello", true)
		});
	}

	{
		State *state = new State("./test-file.h", {"hello"});

		std::string dataBeforeTest = state->file->data[0];

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		std::string dataBeforeUndo = state->file->data[0];
		uint32_t historySizeBeforeUndo = state->file->history.size();

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		output.push_back({
			"undo command not recorded",
			compare((uint32_t)state->file->history.size() == historySizeBeforeUndo, true)
		});

		output.push_back({
			"undo changes file data",
			compare(state->file->data[0] != dataBeforeUndo, true)
		});

		output.push_back({
			"undo sets data to before test",
			compare(state->file->data[0] == dataBeforeTest, true)
		});
	}

	{
		State *state = new State("./test-file.h", {"hello"});
		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');
		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		std::string dataBeforeRedo = state->file->data[0];
		uint32_t historySizeBeforeRedo = state->file->history.size();

		sendKeys(state, ctrl('r'));
		cleanup(state, ctrl('r'));
		history(state, ctrl('r'));

		output.push_back({
			"redo command not recorded",
			compare((uint32_t)state->file->history.size() == historySizeBeforeRedo, true)
		});

		output.push_back({
			"redo changes file data",
			compare(state->file->data[0] != dataBeforeRedo, true)
		});
	}

	{
		State *state = new State("./test-file.h", {"HELLO"});
		state->mode = VISUAL;
		state->visual.row = 0;
		state->visual.col = 4;
		uint32_t initialHistorySize = state->file->history.size();

		sendKeys(state, 'g');
		cleanup(state, 'g');
		history(state, 'g');
		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		output.push_back({
			"visual 'u' command recorded",
			compare((uint32_t)state->file->history.size() > initialHistorySize, true)
		});

		output.push_back({
			"visual 'u' changes data to lowercase",
			compare(state->file->data[0] == "hello", true)
		});
	}

	{
		State *state = new State("./test-file.h", {"hello"});

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		sendKeys(state, 'x');

		output.push_back({
			"flag reset after keypress",
			compare(state->dontComputeHistory, false)
		});
	}

	{
		State *state = new State("./test-file.h", {"hello"});

		sendKeys(state, 'u');

		output.push_back({
			"flag set for undo",
			compare(state->dontComputeHistory, true)
		});
	}

	return {"testHistory", output};
}
