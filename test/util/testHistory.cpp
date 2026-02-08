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

	{
		State *state = new State("./test-file.h", {"original"});
		std::string originalData = state->file->data[0];
		int32_t originalHistoryPosition = state->file->historyPosition;

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		output.push_back({
			"undo at start should not change data",
			compare(state->file->data[0] == originalData, true)
		});
		output.push_back({
			"undo at start should not change historyPosition",
			compare(state->file->historyPosition == originalHistoryPosition, true)
		});
	}

	{
		State *state = new State("./test-file.h", {"original"});
		std::string originalData = state->file->data[0];
		int32_t originalHistoryPosition = state->file->historyPosition;

		sendKeys(state, ctrl('r'));
		cleanup(state, ctrl('r'));
		history(state, ctrl('r'));

		output.push_back({
			"redo when nothing to redo should not change data",
			compare(state->file->data[0] == originalData, true)
		});
		output.push_back({
			"redo when nothing to redo should not change historyPosition",
			compare(state->file->historyPosition == originalHistoryPosition, true)
		});
	}

	{
		State *state = new State("./test-file.h", {"initial"});

		for (int i = 0; i < 10; i++) {
			sendKeys(state, 'x');
			cleanup(state, 'x');
			history(state, 'x');
		}

		std::string dataAfterChanges = state->file->data[0];

		for (int i = 0; i < 10; i++) {
			sendKeys(state, 'u');
			cleanup(state, 'u');
			history(state, 'u');
		}

		output.push_back({
			"mass undo should restore original data",
			compare(state->file->data[0] == "initial", true)
		});

		for (int i = 0; i < 10; i++) {
			sendKeys(state, ctrl('r'));
			cleanup(state, ctrl('r'));
			history(state, ctrl('r'));
		}

		output.push_back({
			"mass redo should restore data after changes",
			compare(state->file->data[0] == dataAfterChanges, true)
		});
	}

	{
		State *state = new State("./test-file.h", {"start"});

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		std::string afterFirst = state->file->data[0];

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		sendKeys(state, ctrl('r'));
		cleanup(state, ctrl('r'));
		history(state, ctrl('r'));

		output.push_back({
			"complex: redo after undo restores change",
			compare(state->file->data[0] == afterFirst, true)
		});

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		output.push_back({
			"complex: double undo goes back to start",
			compare(state->file->data[0] == "start", true)
		});
	}

	{
		State *state = new State("./test-file.h", {"truncation test"});

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		uint32_t historySizeBefore = state->file->history.size();

		sendKeys(state, 'u');
		cleanup(state, 'u');
		history(state, 'u');

		int32_t positionAfterUndo = state->file->historyPosition;

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		output.push_back({
			"truncation: history size should be 2 after truncate",
			compare((uint32_t)state->file->history.size(), (uint32_t)2)
		});

		output.push_back({
			"truncation: position should be at end after new edit",
			compareInt(state->file->historyPosition, 1)
		});
	}

	{
		State *state = new State("./test-file.h", {"overflow test"});

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		std::string originalData = state->file->data[0];
		int32_t originalPosition = state->file->historyPosition;

		for (int i = 0; i < 20; i++) {
			sendKeys(state, 'u');
			cleanup(state, 'u');
			history(state, 'u');
		}

		output.push_back({
			"overflow: position should be -1 after many undos",
			compareInt(state->file->historyPosition, -1)
		});

		for (int i = 0; i < 20; i++) {
			sendKeys(state, ctrl('r'));
			cleanup(state, ctrl('r'));
			history(state, ctrl('r'));
		}

		output.push_back({
			"overflow: position should be at last entry after many redos",
			compareInt(state->file->historyPosition, originalPosition)
		});
	}

	{
		State *state = new State("./test-file.h", {"interleaved"});

		sendKeys(state, 'x');
		cleanup(state, 'x');
		history(state, 'x');

		int32_t pos1 = state->file->historyPosition;

		for (int i = 0; i < 5; i++) {
			sendKeys(state, 'u');
			cleanup(state, 'u');
			history(state, 'u');

			sendKeys(state, ctrl('r'));
			cleanup(state, ctrl('r'));
			history(state, ctrl('r'));
		}

		output.push_back({
			"interleaved: undo/redo cycle returns to same position",
			compareInt(state->file->historyPosition, pos1)
		});
	}

	return {"testHistory", output};
}
