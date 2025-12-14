#include "save.h"
#include <fstream>

void saveFile(State *state)
{
	state->file->lastSave = state->file->historyPosition;
	if (!state->dontSave) {
		std::ofstream file(state->file->filename);
		if (!state->file->data.empty()) {
			for (size_t i = 0; i < state->file->data.size() - 1; ++i) {
				file << state->file->data[i] << "\n";
			}
			file << state->file->data.back();
		}
		file.close();
	}
}
