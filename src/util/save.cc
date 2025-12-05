#include "save.h"
#include <fstream>

void saveFile(State *state)
{
	state->lastSave = state->historyPosition;
	if (!state->dontSave) {
		std::ofstream file(state->filename);
		if (!state->data.empty()) {
			for (size_t i = 0; i < state->data.size() - 1; ++i) {
				file << state->data[i] << "\n";
			}
			file << state->data.back();
		}
		file.close();
	}
}
