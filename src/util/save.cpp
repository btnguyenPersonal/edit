#include "save.h"
#include <fstream>

void saveFile(State *state)
{
	if (!state->dontSave) {
		std::ofstream file(state->file->filename);
		if (!state->file->data.empty()) {
			for (size_t i = 0; i < state->file->data.size(); i++) {
				file << state->file->data[i] << "\n";
			}
			// NOTE(ben): this preserves the state of the trailing newline
			if (!state->file->data.empty() && state->file->data.back().empty()) {
				file.seekp(-1, std::ios::end);
			}
		}
		file.close();
		state->file->newFile = false;
		state->file->lastSave = state->file->historyPosition;
	}
}
