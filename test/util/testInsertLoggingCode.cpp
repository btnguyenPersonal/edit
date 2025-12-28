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
		State *state = new State("./test-file.h", { "test", "printf(\"hi\\n\");", "printf(\"normal%d\\n\", d);" });
		toggleLoggingCode(state, "test");
		output.push_back({ "getLoggingCode c 5", compare(state->file->data, { "test", "printf(\"1 test: %d\\n\", test);", "printf(\"hi\\n\");", "printf(\"normal%d\\n\", d);" }) });
		toggleLoggingCode(state, "test");
		output.push_back({ "getLoggingCode c 6", compare(state->file->data, { "test", "printf(\"hi\\n\");", "printf(\"normal%d\\n\", d);" }) });
		toggleLoggingCode(state, "test");
		toggleLoggingCode(state, "test2");
		toggleLoggingCode(state, "test3");
		toggleLoggingCode(state, "test4");
		state->file->row = 6;
		toggleLoggingCode(state, "vec()");
		output.push_back({ "getLoggingCode c 7", compare(state->file->data, {
			"test",
			"printf(\"1 test4: %d\\n\", test4);",
			"printf(\"1 test3: %d\\n\", test3);",
			"printf(\"1 test2: %d\\n\", test2);",
			"printf(\"1 test: %d\\n\", test);",
			"printf(\"hi\\n\");",
			"printf(\"normal%d\\n\", d);",
	                "printf(\"7 vec(): %d\\n\", vec());"
		}) });
		removeAllLoggingCode(state);
		output.push_back({ "removeAllLoggingCode c 0", compare(state->file->data, { "test", "printf(\"hi\\n\");", "printf(\"normal%d\\n\", d);" }) });
	}

	{
		State *state = new State("./test-file.h", { "function (param) {", "    return 0;", "}" });
		toggleLoggingCode(state, "param");
		output.push_back({ "getLoggingCode c 8", compare(state->file->data, { "function (param) {", "    printf(\"1 param: %d\\n\", param);", "    return 0;", "}" }) });
		toggleLoggingCode(state, "param");
		output.push_back({ "getLoggingCode c 9", compare(state->file->data, { "function (param) {", "    return 0;", "}" }) });
		toggleLoggingCode(state, "\"\\all the escape/sequenc%s in one thing\\\"");
		output.push_back({ "getLoggingCode c 9", compare(state->file->data, { "function (param) {", "    printf(\"1 \\\"\\\\all the escape/sequenc%%s in one thing\\\\\\\": %d\\n\", \"\\all the escape/sequenc%s in one thing\\\");", "    return 0;", "}" }) });
		removeAllLoggingCode(state);
		output.push_back({ "removeAllLoggingCode c 1", compare(state->file->data, { "function (param) {", "    return 0;", "}" }) });
	}

	{
		State *state = new State("./test-file.js", { "" });
		output.push_back({ "getLoggingCode js 0", compare(getLoggingCode(state, ""), "") });
		output.push_back({ "getLoggingCode js 1", compare(getLoggingCode(state, "temp"), "console.log('1 temp', temp);") });
		output.push_back({ "getLoggingCode js 2", compare(getLoggingCode(state, "temp + y"), "console.log('1 temp + y', temp + y);") });
		output.push_back({ "getLoggingCode js 3", compare(getLoggingCode(state, "temp\\"), "console.log('1 temp\\', temp\\);") });
		output.push_back({ "getLoggingCode js 4", compare(getLoggingCode(state, "'temp'"), "console.log('1 \\'temp\\'', 'temp');") });
	}

	{
		State *state = new State("./test-file.js", { "" });
		output.push_back({ "getLoggingCode js 5", compare(getLoggingCode(state, ""), "") });
		output.push_back({ "getLoggingCode js 6", compare(getLoggingCode(state, "temp"), "console.log('1 temp', temp);") });
		output.push_back({ "getLoggingCode js 7", compare(getLoggingCode(state, "temp + y"), "console.log('1 temp + y', temp + y);") });
		output.push_back({ "getLoggingCode js 8", compare(getLoggingCode(state, "temp\\"), "console.log('1 temp\\', temp\\);") });
		output.push_back({ "getLoggingCode js 9", compare(getLoggingCode(state, "'temp'"), "console.log('1 \\'temp\\'', 'temp');") });
	}

	return { "test/util/testInsertLoggingCode.cpp", output };
}

