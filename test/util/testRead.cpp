#include "../../src/util/read.h"
#include "../../src/util/state.h"
#include "../../src/util/compare.h"
#include "../test.h"

struct testSuiteRun testRead() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		system("rm ./test-file.h; touch ./test-file.h");
		output.push_back({ "it should read an empty file and give back an empty line", compare(readFile("./test-file.h"), {}) });
	}

	{
		system("rm ./test-file.h; echo \"Hello World!\" > ./test-file.h");
		output.push_back({ "it should read a one line file and give back the line", compare(readFile("./test-file.h"), {"Hello World!"}) });
	}

	return { "test/util/testRead.cpp", output };
}
