#include "dirSplit.h"
#include "string.h"

struct DirSplit getCurrentDirSplit(std::string currentPathQuery) {
	DirSplit output = {};
	if (currentPathQuery.length() == 1) {
		output.lastDirectory = currentPathQuery;
	}
	for (int32_t i = currentPathQuery.length() - 1; i >= 0; i--) {
		if (currentPathQuery[i] == '/') {
			output.lastDirectory = currentPathQuery.substr(0, i);
			output.currentUncompleted = safeSubstring(currentPathQuery, i + 1);
			if (output.lastDirectory == "") {
				output.lastDirectory = "/";
			}
			break;
		}
	}
	return output;
}
