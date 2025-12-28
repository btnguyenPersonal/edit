#include "testInsertLoggingCode.h"

struct testSuiteRun testInsertLoggingCode()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		output.push_back({ "getLoggingCode c 0", compare(getLoggingCode(state, ""), "") });
		output.push_back({ "getLoggingCode c 1", compare(getLoggingCode(state, "temp"), "printf(\"1 temp: %d\\n\", temp);") });
		output.push_back({ "getLoggingCode c 2", compare(getLoggingCode(state, "jojojo + jajaja"), "printf(\"1 jojojo + jajaja: %d\\n\", jojojo + jajaja);") });
		output.push_back({ "getLoggingCode c 3", compare(getLoggingCode(state, "printf(\"1 temp: %d\\n\", temp);"), "printf(\"1 printf(\\\"1 temp: %%d\\\\n\\\", temp);: %d\\n\", printf(\"1 temp: %d\\n\", temp););") });
		state->file->row = 999;
		output.push_back({ "getLoggingCode c 4", compare(getLoggingCode(state, "temp"), "printf(\"1000 temp: %d\\n\", temp);") });
	}

	{
		State *state = new State("./test-file.js", { "" });
		output.push_back({ "getLoggingCode js 0", compare(getLoggingCode(state, ""), "") });
		output.push_back({ "getLoggingCode js 1", compare(getLoggingCode(state, "temp"), "console.log('1 temp', temp);") });
		output.push_back({ "getLoggingCode js 2", compare(getLoggingCode(state, "temp + y"), "console.log('1 temp + y', temp + y);") });
		output.push_back({ "getLoggingCode js 3", compare(getLoggingCode(state, "temp\\"), "console.log('1 temp\\', temp\\);") });
		output.push_back({ "getLoggingCode js 4", compare(getLoggingCode(state, "'temp'"), "console.log('1 \\'temp\\'', 'temp');") });
	}

	return { "test/util/testInsertLoggingCode.cpp", output };
}

