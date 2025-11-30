#include "../../src/util/save.h"
#include "../../src/util/read.h"
#include "../../src/util/state.h"
#include "../../src/util/compare.h"
#include "../test.h"

struct testSuiteRun testSave() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		saveFile(state);
		output.push_back({ "it should save the file in the trivial case", compare(readFile("./test-file.h"), {"Hello world!"}) });
	}

	{
		State *state = new State("./test-file.h", {});
		saveFile(state);
		output.push_back({ "it should empty the file if nothing is passed", compare(readFile("./test-file.h"), {""}) });
	}

	return { "test/util/testSave.cpp", output };
}
