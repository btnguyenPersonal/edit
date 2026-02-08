#include "ratchet.h"
#include "../../src/util/external.h"

struct testSuiteRun ratchet() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		std::string matches = runCommand("rg \"file->data\" src --vimgrep | wc -l");
		output.push_back({
			"don't allow more file->data direct accesses (if this count is lower hooray! update number and enjoy)\n",
			compare((uint32_t)stoi(matches), (uint32_t)425)
		});
	}

	return {"test/util/ratchet.cpp", output};
}

