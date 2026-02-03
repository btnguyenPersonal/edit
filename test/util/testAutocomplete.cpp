#include "testAutocomplete.h"

struct testSuiteRun testAutocomplete() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {""});
		output.push_back({"autocomplete on an empty string returns an empty string", compare(autocomplete(state, ""), std::string(""))});
	}

	{
		State *state = new State("./test-file.h", {""});
		output.push_back({"autocomplete on an empty file returns an empty string", compare(autocomplete(state, "a"), std::string(""))});
	}

	{
		State *state = new State("./test-file.h", {"", "abc"});
		output.push_back({"autocomplete on the trivial case", compare(autocomplete(state, "a"), std::string("bc"))});
	}

	{
		State *state = new State("./test-file.h", {"", "abc", "abc"});
		output.push_back({"autocomplete on no matches found returns an empty string", compare(autocomplete(state, "b"), std::string(""))});
	}

	{
		State *state = new State("./test-file.h", {"", "abc", "acb"});
		output.push_back({"autocomplete on multiple tied matches, chooses the first one", compare(autocomplete(state, "a"), std::string("bc"))});
	}

	{
		State *state = new State("./test-file.h", {"", "abc", "acb", "acb"});
		output.push_back({"autocomplete on match that is not the first, chooses the most common match", compare(autocomplete(state, "a"), std::string("cb"))});
	}

	{
		State *state = new State("./test-file.h", {"", "abc", "acb", "acb"});
		output.push_back({"autocomplete on match that is the same as the current string, returns empty string", compare(autocomplete(state, "acb"), std::string(""))});
	}

	{
		State *state = new State("./test-file.h", {"", "                    abc                                ", "", ""});
		output.push_back({"autocomplete string that is in middle of line", compare(autocomplete(state, "a"), std::string("bc"))});
	}

	{
		State *state = new State("./test-file.h", {"                    abc                                ", "                    abc                                ", "", ""});
		output.push_back({"autocomplete string that is on the current line", compare(autocomplete(state, "a"), std::string("bc"))});
	}

	{
		State *state = new State("./test-file.h", {"abc", "abcdefghijklmnopqrstuvwxyz", "", ""});
		output.push_back({"autocomplete long string currently typing", compare(autocomplete(state, "abc"), std::string("defghijklmnopqrstuvwxyz"))});
	}

	{
		State *state = new State("./test-file.h", {"abcdefghijklmnopqrstuvwxy", "abcdefghijklmnopqrstuvwxyz", "", ""});
		output.push_back({"autocomplete long string almost done typing", compare(autocomplete(state, "abcdefghijklmnopqrstuvwxy"), std::string("z"))});
	}

	{
		State *state = new State("./test-file.h", {"a_", "a_b_c_d", "", ""});
		output.push_back({"autocomplete with underscores", compare(autocomplete(state, "a_"), std::string("b_c_d"))});
	}

	{
		State *state = new State("./test-file.h", {"a(", "a(b(c(d", "", ""});
		output.push_back({"autocomplete doesn't work with (", compare(autocomplete(state, "a("), std::string(""))});
	}

	{
		State *state = new State("./test-file.h", {"fun(", "function2()", "function()", "function()", "function2()"});
		output.push_back({"autocomplete picks the smaller one", compare(autocomplete(state, "fun"), std::string("ction"))});
	}

	{
		State *state = new State("./test-file.h", {"function(", "function2()", "function()", "function()", "function2()"});
		output.push_back({"autocomplete picks the smaller non-current one", compare(autocomplete(state, "function"), std::string("2"))});
	}

	return {"test/util/testAutocomplete.cpp", output};
}
