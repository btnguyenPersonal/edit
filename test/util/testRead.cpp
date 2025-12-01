#include "../../src/util/save.h"
#include "../../src/util/read.h"
#include "../../src/util/state.h"
#include "../../src/util/compare.h"
#include "../test.h"

struct testSuiteRun testRead() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		system("rm ./test-file.h");
		system("touch ./test-file.h");
		output.push_back({ "it should read an empty file and give back an empty line", compare(readFile("./test-file.h"), {}) });
	}

	{
		system("rm ./test-file.h");
		system("echo \"Hello World!\" > ./test-file.h");
		output.push_back({ "it should read a one line file and give back the line", compare(readFile("./test-file.h"), {"Hello World!"}) });
	}

	{
		system("rm ./test-file.h");
		uint32_t num = 100000;
		std::vector<std::string> f = {};
		for (uint32_t i = 0; i < num; i++) {
			f.push_back(std::to_string(i));
		}
		State *state = new State("./test-file.h", f);
		saveFile(state);
		output.push_back({ "it should read a big file correctly", compare(readFile("./test-file.h"), f) });
	}

	return { "test/util/testRead.cpp", output };
}
