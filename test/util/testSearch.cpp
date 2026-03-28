#include "testSearch.h"

struct testSuiteRun testSearch() {
	std::vector<struct testRun> output = std::vector<struct testRun>();
	
	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		setSearchResultCurrentLine(state, "0");
		output.push_back({"sameLine finds it", compare(state->file->col, 6)});
	}
	
	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		setSearchResultCurrentLine(state, "z");
		output.push_back({"sameLine doesnt find it", compare(state->file->col, 0)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->search.query = "z";
		searchNextResult(state, false);
		output.push_back({"searchNextResult doesnt find it", compare(state->file->row, 0)});
		output.push_back({"searchNextResult doesnt find it", compare(state->file->col, 0)});
		output.push_back({"searchNextResult doesnt find it", compare(state->searchFail, true)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "z";
		searchNextResult(state, false);
		output.push_back({"searchNextResult doesnt find it and wraps around", compare(state->file->row, 3)});
		output.push_back({"searchNextResult doesnt find it and wraps around", compare(state->file->col, 6)});
		output.push_back({"searchNextResult doesnt find it and wraps around", compare(state->searchFail, true)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "search";
		searchNextResult(state, false);
		output.push_back({"searchNextResult finds it and wraps around", compare(state->file->row, 0)});
		output.push_back({"searchNextResult finds it and wraps around", compare(state->file->col, 0)});
		output.push_back({"searchNextResult finds it and wraps around", compare(state->searchFail, false)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "ch1";
		searchNextResult(state, false);
		output.push_back({"searchNextResult finds it and wraps around", compare(state->file->row, 1)});
		output.push_back({"searchNextResult finds it and wraps around", compare(state->file->col, 4)});
		output.push_back({"searchNextResult finds it and wraps around", compare(state->searchFail, false)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "ch3";
		searchNextResult(state, false);
		output.push_back({"searchNextResult finds something on cursor", compare(state->file->row, 3)});
		output.push_back({"searchNextResult finds something on cursor", compare(state->file->col, 4)});
		output.push_back({"searchNextResult finds something on cursor", compare(state->searchFail, false)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "";
		searchNextResult(state, false);
		output.push_back({"searchNextResult no query", compare(state->file->row, 3)});
		output.push_back({"searchNextResult no query", compare(state->file->col, 6)});
		output.push_back({"searchNextResult no query", compare(state->searchFail, true)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->search.query = "z";
		searchNextResult(state, true);
		output.push_back({"reverse searchNextResult doesnt find it", compare(state->file->row, 0)});
		output.push_back({"reverse searchNextResult doesnt find it", compare(state->file->col, 0)});
		output.push_back({"reverse searchNextResult doesnt find it", compare(state->searchFail, true)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "z";
		searchNextResult(state, true);
		output.push_back({"reverse searchNextResult doesnt find it and wraps around", compare(state->file->row, 3)});
		output.push_back({"reverse searchNextResult doesnt find it and wraps around", compare(state->file->col, 6)});
		output.push_back({"reverse searchNextResult doesnt find it and wraps around", compare(state->searchFail, true)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "search";
		searchNextResult(state, true);
		output.push_back({"reverse searchNextResult finds it and wraps around", compare(state->file->row, 3)});
		output.push_back({"reverse searchNextResult finds it and wraps around", compare(state->file->col, 0)});
		output.push_back({"reverse searchNextResult finds it and wraps around", compare(state->searchFail, false)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "ch1";
		searchNextResult(state, true);
		output.push_back({"reverse searchNextResult finds it and wraps around", compare(state->file->row, 1)});
		output.push_back({"reverse searchNextResult finds it and wraps around", compare(state->file->col, 4)});
		output.push_back({"reverse searchNextResult finds it and wraps around", compare(state->searchFail, false)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "ch3";
		searchNextResult(state, true);
		output.push_back({"reverse searchNextResult finds something on cursor", compare(state->file->row, 3)});
		output.push_back({"reverse searchNextResult finds something on cursor", compare(state->file->col, 4)});
		output.push_back({"reverse searchNextResult finds something on cursor", compare(state->searchFail, false)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 6;
		state->search.query = "";
		searchNextResult(state, true);
		output.push_back({"reverse searchNextResult no query", compare(state->file->row, 3)});
		output.push_back({"reverse searchNextResult no query", compare(state->file->col, 6)});
		output.push_back({"reverse searchNextResult no query", compare(state->searchFail, true)});
	}

	{
		State *state = new State("/tmp/test-file.h", {
			"search0",
			"search1",
			"search2",
			"search3"
		});
		state->file->row = 3;
		state->file->col = 0;
		state->search.query = "search";
		setSearchResultReverse(state, true);
		output.push_back({"setSearchResultReverse allowCurrent", compare(state->file->row, 3)});
		output.push_back({"setSearchResultReverse allowCurrent", compare(state->file->col, 0)});
		output.push_back({"setSearchResultReverse allowCurrent", compare(state->searchFail, false)});
	}

	return {"test/util/testSearch.cpp", output};
}
