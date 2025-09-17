#include "sendDiffKeys.h"

void upLog(State* state)
{
	if (state->logIndex > 0) {
		state->logIndex--;
	}
}

void downLog(State* state)
{
	if (state->logIndex + 1 < state->logLines.size()) {
		state->logIndex++;
	}
}

void upDiff(State* state)
{
	if (state->diffIndex > 0) {
		state->diffIndex--;
	}
}

void downDiff(State* state)
{
	if (state->diffIndex + 1 < state->diffLines.size()) {
		state->diffIndex++;
	}
}

void sendDiffKeys(State *state, int32_t c)
{
	if (c == 'q') {
		state->mode = SHORTCUTS;
	} else if (c == 27) { // ESC
		if (state->prevKeys != "") {
			state->prevKeys = "";
		} else if (state->viewingDiff) {
			state->logLines = getLogLines(state);
			state->viewingDiff = false;
			state->diffIndex = 0;
		} else {
			state->mode = SHORTCUTS;
		}
	} else if (c == ctrl('u') || c == 'u') {
		for (uint32_t i = 0; i < state->maxY / 2; i++) {
			if (state->viewingDiff) {
				upDiff(state);
			} else {
				upLog(state);
			}
		}
	} else if (c == ctrl('d') || c == 'd') {
		for (uint32_t i = 0; i < state->maxY / 2; i++) {
			if (state->viewingDiff) {
				downDiff(state);
			} else {
				downLog(state);
			}
		}
	} else if (c == 'h') {
		if (state->viewingDiff) {
			downLog(state);
			state->diffIndex = 0;
			state->diffLines = getDiffLines(state);
		}
	} else if (c == 'l') {
		if (state->viewingDiff) {
			upLog(state);
			state->diffIndex = 0;
			state->diffLines = getDiffLines(state);
		}
	} else if (c == 'j') {
		if (state->viewingDiff) {
			downDiff(state);
		} else {
			downLog(state);
		}
	} else if (c == 'G') {
		if (state->diffLines.size() > 0) {
			state->diffIndex = state->diffLines.size() - 1;
		}
	} else if (c == 'g') {
		if (state->prevKeys == "g") {
			state->diffIndex = 0;
			state->prevKeys = "";
		} else {
			state->prevKeys = "g";
		}
	} else if (c == 'k') {
		if (state->viewingDiff) {
			upDiff(state);
		} else {
			upLog(state);
		}
	} else if (c == '\n') {
		if (state->viewingDiff) {
			std::string num = "";
			int32_t linesNet = 0;
			std::string file = "";
			bool found = false;
			for (int32_t i = state->diffIndex; i >= 0; i--) {
				if (!found && state->diffLines[i].length() > 0 && state->diffLines[i][0] == '+') {
					linesNet++;
				} else if (found && state->diffLines[i].length() > 2 && state->diffLines[i][0] == '+' && state->diffLines[i][1] == '+' && state->diffLines[i][2] == '+') {
					std::string line = state->diffLines[i];
					file = safeSubstring(line, 6);
					break;
				} else if (state->diffLines[i].length() > 1 && state->diffLines[i][0] == '@' && state->diffLines[i][1] == '@') {
					std::string line = state->diffLines[i];
					for (uint32_t i = 0; i < line.length(); i++) {
						if (!found && line[i] == '+') {
							found = true;
						} else if (found && line[i] == ',') {
							break;
						} else if (found) {
							num += line[i];
						}
					}
				}
			}
			if (num != "" && file != "") {
				state->changeFile(file);
				state->row = std::stoi(num) + linesNet + 1;
				centerScreen(state);
				state->windowPosition.row -= 2;
			}
		} else {
			state->diffLines = getDiffLines(state);
			state->viewingDiff = true;
		}
	}
}
