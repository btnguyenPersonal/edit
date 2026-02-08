#include "testSave.h"

struct testSuiteRun testSave() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		system("rm -f /tmp/test-file.h");
		State *state = new State("/tmp/test-file.h", {"Hello world!"});
		state->dontSave = true;
		saveFile(state);
		output.push_back({"saveFile should not save the file if dontSave is true", compare(readFile("/tmp/test-file.h"), {""})});
	}

	{
		system("rm -f /tmp/test-file.h");
		State *state = new State("/tmp/test-file.h", {"Hello world!"});
		state->dontSave = false;
		saveFile(state);
		output.push_back({"saveFile should save the file in the trivial case", compare(readFile("/tmp/test-file.h"), {"Hello world!"})});
	}

	{
		system("rm -f /tmp/test-file.h");
		State *state = new State("/tmp/test-file.h", {"\n", ""});
		state->dontSave = false;
		saveFile(state);
		output.push_back({"saveFile should save the file with newlines when they are passed down", compare(readFile("/tmp/test-file.h"), {"", "", ""})});
	}

	{
		system("rm -f /tmp/test-file.h");
		State *state = new State("/tmp/test-file.h", {});
		state->dontSave = false;
		saveFile(state);
		output.push_back({"saveFile should empty the file if nothing is passed", compare(readFile("/tmp/test-file.h"), {""})});
	}

	return {"test/util/testSave.cpp", output};
}
