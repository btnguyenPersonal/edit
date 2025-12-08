#include "testAutocomplete.h"

struct testSuiteRun testAutocomplete()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", { "" });
		output.push_back({ "autocomplete on an empty string returns an empty string", compare(autocomplete(state, ""), std::string("")) });
	}

	{
		State *state = new State("./test-file.h", { "" });
		output.push_back({ "autocomplete on an empty file returns an empty string", compare(autocomplete(state, "a"), std::string("")) });
	}

	{
		State *state = new State("./test-file.h", { "", "abc" });
		output.push_back({ "autocomplete on the trivial case", compare(autocomplete(state, "a"), std::string("bc")) });
	}

	{
		State *state = new State("./test-file.h", { "", "abc", "abc" });
		output.push_back({ "autocomplete on no matches found returns an empty string", compare(autocomplete(state, "b"), std::string("")) });
	}

	{
		State *state = new State("./test-file.h", { "", "abc", "acb" });
		output.push_back({ "autocomplete on multiple tied matches, chooses the first one", compare(autocomplete(state, "a"), std::string("bc")) });
	}

	{
		State *state = new State("./test-file.h", { "", "abc", "acb", "acb" });
		output.push_back({ "autocomplete on match that is not the first, chooses the most common match", compare(autocomplete(state, "a"), std::string("cb")) });
	}

	{
		State *state = new State("./test-file.h", { "", "abc", "acb", "acb" });
		output.push_back({ "autocomplete on match that is not the first, chooses the most common match", compare(autocomplete(state, "a"), std::string("dddd")) });
	}

	return { "test/util/testAutocomplete.cc", output };
}
