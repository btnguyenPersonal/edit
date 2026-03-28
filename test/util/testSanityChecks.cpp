#include "testSanityChecks.h"
#include "testFileExplorerNode.h"

struct testSuiteRun testSanityChecks() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		State *state = new State("./test-file.h", {});
		fixColOverMax(state);
		output.push_back({"fixColOverMax should add an empty line if the data is empty", compare(state->file->data, {""})});
	}

	{
		State *state = new State("./test-file.h", {""});
		fixColOverMax(state);
		output.push_back({"fixColOverMax should set col to 0 if col is already 0 and the line is empty", compare(state->file->col, 0)});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->file->col = 1;
		fixColOverMax(state);
		output.push_back({"fixColOverMax should set col to 0 if col is 1 and the line is empty", compare(state->file->col, 0)});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->file->row = 1;
		fixColOverMax(state);
		output.push_back({"fixColOverMax should set row to 0 if row is 1 and the row does not exist", compare(state->file->row, 0)});
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->file->col = 1000;
		fixColOverMax(state);
		output.push_back({"fixColOverMax should set col to the end in the trivial case", compare(state->file->col, std::string("Hello world!").length())});
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->file->col = 13;
		fixColOverMax(state);
		output.push_back({"fixColOverMax should set col to the end when one after the end", compare(state->file->col, std::string("Hello world!").length())});
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->file->col = 11;
		fixColOverMax(state);
		output.push_back({"fixColOverMax should not set col to the end when one before the end", compare(state->file->col, 11)});
	}

	{
		State *state = new State("./test-file.h", {"Hello world!"});
		state->file->col = 0;
		fixColOverMax(state);
		output.push_back({"fixColOverMax should not set col to the end when at the start", compare(state->file->col, 0)});
	}

	{
		State *state = new State("./test-file.h", {});
		sanityCheckDocumentEmpty(state);
		output.push_back({"sanityCheckDocumentEmpty should insert an empty string when document is empty", compare(state->file->data, {""})});
	}

	{
		State *state = new State("./test-file.h", {"hi"});
		sanityCheckDocumentEmpty(state);
		output.push_back({"sanityCheckDocumentEmpty should not insert an empty string when document is not empty", compare(state->file->data, {"hi"})});
	}

	{
		State *state = new State("./test-file.h", {"hi"});
		sanityCheckRowOutOfBounds(state);
		output.push_back({"sanityCheckRowOutOfBounds should set row to 0 when row is not out of bounds", compare(state->file->row, 0)});
	}

	{
		State *state = new State("./test-file.h", {"hi"});
		state->file->row = 10;
		sanityCheckRowOutOfBounds(state);
		output.push_back({"sanityCheckRowOutOfBounds should set row to 0 when row is out of bounds and only one row", compare(state->file->row, 0)});
	}

	{
		State *state = new State("./test-file.h", {"hi", "hi2", "hi3"});
		state->file->row = 3;
		sanityCheckRowOutOfBounds(state);
		output.push_back({"sanityCheckRowOutOfBounds should set row to length - 1 when row is one over the length", compare(state->file->row, 2)});
	}

	{
		State *state = new State("./test-file.h", {"hi", "hi2", "hi3"});
		state->file->row = 1;
		sanityCheckRowOutOfBounds(state);
		output.push_back({"sanityCheckRowOutOfBounds should not set row to length - 1 when row is one under the length", compare(state->file->row, 1)});
	}

	{
		Query query = {"", 0, 5};
		sanityCheckQuery(query, 0);
		output.push_back({"sanityCheckQuery should set selection to 0 if len is 0", compare(query.selection, 0)});
	}

	{
		Query query = {"", 0, 5};
		sanityCheckQuery(query, 5);
		output.push_back({"sanityCheckQuery should set selection to 4 if len is 5", compare(query.selection, 4)});
	}

	{
		Query query = {"", 0, 5};
		sanityCheckQuery(query, 100);
		output.push_back({"sanityCheckQuery should keep selection at 5 if len is 100", compare(query.selection, 5)});
	}

	{
		Query query = {"", 0, 5};
		sanityCheckQuery(query, 100);
		output.push_back({"sanityCheckQuery should keep selection at 5 if len is 100", compare(query.selection, 5)});
	}

	{
		State *state = new State("./test-file.h", {});
		state->grep.selection = 100;
		for (uint32_t i = 0; i < 6; i++) {
			state->grepOutput.push_back({"", 0, ""});
		}
		sanityCheckGrepSelection(state);
		output.push_back({"sanityCheckGrepSelection should set grep.selection to 5 if length is 6", compare(state->grep.selection, 5)});
	}

	{
		State *state = new State("./test-file.h", {});
		state->find.selection = 100;
		for (uint32_t i = 0; i < 6; i++) {
			state->findOutput.push_back("");
		}
		sanityCheckFindSelection(state);
		output.push_back({"sanityCheckFindSelection should set find.selection to 5 if length is 6", compare(state->find.selection, 5)});
	}

	{
		State *state = new State("./test-file.h", {});
		state->file->row = 5;
		sanityCheckRowOutOfBounds(state);
		output.push_back({"sanityCheckRowOutOfBounds row out of bounds", compare(state->file->row, 0)});
	}

	{
		State *state = new State("./test-file.h", {"testing"});
		state->file->col = 1000;
		resetValidCursorState(state);
		output.push_back({"resetValidCursorState col out of bounds", compare(state->file->col, 6)});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->file->col = 1000;
		resetValidCursorState(state);
		output.push_back({"resetValidCursorState col out of bounds on a zero length string", compare(state->file->col, 0)});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->explorer.windowLine = -1;
		sanityCheckExplorer(state);
		output.push_back({"sanityCheckExplorer windowLine negative", compareInt((int32_t)state->explorer.windowLine, (int32_t)0)});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->explorer.index = -1;
		sanityCheckExplorer(state);
		output.push_back({"sanityCheckExplorer index negative", compareInt((int32_t)state->explorer.index, (int32_t)0)});
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/subdir1/subdir2/subdir3");
		system("touch /tmp/test-explorer/subdir1/a.txt");
		system("touch /tmp/test-explorer/subdir1/subdir2/a.txt");
		system("touch /tmp/test-explorer/subdir1/subdir2/subdir3/a.txt");

		State *state = new State("./test-file.h", {""});
		state->explorer.root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		state->explorer.root->expand("subdir1/subdir2/subdir3");

		state->explorer.index = 6;
		sanityCheckExplorer(state);

		output.push_back({"sanityCheckExplorer index negative", compareInt((int32_t)state->explorer.index, (int32_t)5)});
	}

	return {"test/util/testSanityChecks.cpp", output};
}
