#include "testRead.h"

struct testSuiteRun testRead() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		system("rm -f ./test-file.h");
		system("touch ./test-file.h");
		output.push_back({"readFile should read an empty file and give back an empty line", compare(readFile("./test-file.h"), {""})});
	}

	{
		system("rm -f ./test-file.h");
		system("echo \"Hello World!\" > ./test-file.h");
		output.push_back({"readFile should read a one line file and give back the line", compare(readFile("./test-file.h"), {"Hello World!"})});
	}

	{
		system("rm -f ./test-file.h");
		uint32_t num = 1000;
		std::vector<std::string> f = {};
		for (uint32_t i = 0; i < num; i++) {
			f.push_back(std::to_string(i));
		}
		State *state = new State("./test-file.h", f);
		state->dontSave = false;
		saveFile(state);
		output.push_back({"readFile should read a big file correctly", compare(readFile("./test-file.h"), f)});
	}

	return {"test/util/testRead.cpp", output};
}
