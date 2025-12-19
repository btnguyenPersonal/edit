#include "testSave.h"

struct testSuiteRun testSave()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		system("rm -f ./test-file.h");
		State *state = new State("./test-file.h", { "Hello world!" });
		state->dontSave = true;
		saveFile(state);
		output.push_back({ "saveFile should not save the file if dontSave is true", compare(readFile("./test-file.h"), { "" }) });
	}

	{
		system("rm -f ./test-file.h");
		State *state = new State("./test-file.h", { "Hello world!" });
		state->dontSave = false;
		saveFile(state);
		output.push_back({ "saveFile should save the file in the trivial case", compare(readFile("./test-file.h"), { "Hello world!" }) });
	}

	{
		system("rm -f ./test-file.h");
		State *state = new State("./test-file.h", { "\n", "" });
		state->dontSave = false;
		saveFile(state);
		output.push_back({ "saveFile should save the file with newlines when they are passed down", compare(readFile("./test-file.h"), { "", "" }) });
	}

	{
		system("rm -f ./test-file.h");
		State *state = new State("./test-file.h", {});
		state->dontSave = false;
		saveFile(state);
		output.push_back({ "saveFile should empty the file if nothing is passed", compare(readFile("./test-file.h"), { "" }) });
	}

	return { "test/util/testSave.cpp", output };
}
