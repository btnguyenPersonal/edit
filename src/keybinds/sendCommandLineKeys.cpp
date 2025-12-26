#include "sendCommandLineKeys.h"
#include "../util/clipboard.h"
#include "../util/render.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/state.h"
#include "../util/save.h"
#include "../util/search.h"
#include "../util/fileops.h"
#include <ncurses.h>
#include <string>
#include <vector>

struct searchReplace {
	std::string first;
	std::string second;
};

struct searchReplace getSearchReplace(std::string query)
{
	struct searchReplace output = { "", "" };
	uint32_t slashesFound = 0;
	for (uint32_t i = 0; i < query.length(); i++) {
		if (query[i] == '/') {
			slashesFound++;
		} else if (slashesFound == 1) {
			output.first += query[i];
		} else if (slashesFound == 2) {
			output.second += query[i];
		}
	}
	return output;
}

void evaluateCommandLineQuery(State *state)
{
	if (state->commandLine.query == "q!") {
		if (!state->dontSave) {
			endwin();
			exit(0);
		}
	} else if (state->commandLine.query == "q") {
		if (!state->dontSave) {
			if (state->file->lastSave != state->file->historyPosition) {
				state->status = "unsaved changes for file: " + state->file->filename;
			} else {
				endwin();
				exit(0);
			}
		}
	} else if (state->commandLine.query == "w") {
		trimTrailingWhitespace(state);
		saveFile(state);
	} else if (state->commandLine.query == "wq" || state->commandLine.query == "x") {
		if (!state->dontSave) {
			trimTrailingWhitespace(state);
			saveFile(state);
			endwin();
			exit(0);
		}
	} else if (state->commandLine.query.substr(0, 1) == "e") {
		if (state->commandLine.query.length() > 2) {
			locateFile(state, safeSubstring(state->commandLine.query, 2), { "" });
		}
	} else if (state->commandLine.query.substr(0, 1) == "s") {
		struct searchReplace temp = getSearchReplace(state->commandLine.query);
		replaceCurrentLine(state, temp.first, temp.second);
	} else if (state->commandLine.query.substr(0, 2) == "gs") {
		struct searchReplace temp = getSearchReplace(state->commandLine.query);
		if (!state->dontSave) {
			renderScreen(state);
			replaceAllGlobally(state, temp.first, temp.second);
		}
	} else if (state->commandLine.query.substr(0, 2) == "%s") {
		struct searchReplace temp = getSearchReplace(state->commandLine.query);
		replaceAll(state, temp.first, temp.second);
	} else if (state->commandLine.query == "retab") {
		if (state->options.indent_style == "tab") {
			replaceAll(state, "    ", "\t");
		} else {
			replaceAll(state, "\t", std::string(state->options.indent_size, ' '));
		}
	} else if (isNumber(state->commandLine.query)) {
		uint32_t number = buildNumberFromString(state->commandLine.query);
		if (number > 0) {
			state->file->row = number - 1;
		} else {
			state->file->row = 0;
		}
	}
}

void autocompleteCommandLinePath(State *state, bool reverse)
{
	if (state->commandLine.query.length() > 1 && safeSubstring(state->commandLine.query, 0, 2) == "e ") {
		std::string currentPathQuery = state->commandLine.query.substr(2);
		struct DirSplit dirSplit = getCurrentDirSplit(state, currentPathQuery);
		std::string currentUncompleted = dirSplit.currentUncompleted;
		std::string lastDirectory = dirSplit.lastDirectory;
		std::vector<std::string> filesInLastDirectory;
		if (lastDirectory != "" && std::filesystem::exists(lastDirectory) && std::filesystem::is_directory(lastDirectory)) {
			if (state->commandLineState.lastDirectory != lastDirectory) {
				state->commandLineState = { 0, currentUncompleted, lastDirectory };
			}
			int32_t skipsLeft = state->commandLineState.skips;
			for (const auto &entry : std::filesystem::directory_iterator(lastDirectory)) {
				filesInLastDirectory.push_back(entry.path());
			}
			for (uint32_t i = 0; i < filesInLastDirectory.size(); i++) {
				for (int32_t j = filesInLastDirectory[i].length() - 1; j >= 0; j--) {
					if (filesInLastDirectory[i][j] == '/') {
						std::string autocompleteFile = safeSubstring(filesInLastDirectory[i], j + 1);
						if (autocompleteFile.length() >= state->commandLineState.currentUncompleted.length()) {
							bool good = true;
							for (uint32_t k = 0; k < state->commandLineState.currentUncompleted.length(); k++) {
								if (autocompleteFile[k] != state->commandLineState.currentUncompleted[k]) {
									good = false;
									break;
								}
							}
							if (good) {
								if (reverse) {
									if (skipsLeft <= 2) {
										state->commandLine.query = std::string("e ") + filesInLastDirectory[i];
										state->commandLine.cursor = state->commandLine.query.length();
										if (skipsLeft > 0) {
											state->commandLineState.skips--;
										}
										return;
									} else {
										skipsLeft--;
									}
								} else {
									if (skipsLeft <= 0) {
										state->commandLine.query = std::string("e ") + filesInLastDirectory[i];
										state->commandLine.cursor = state->commandLine.query.length();
										state->commandLineState.skips++;
										return;
									} else {
										skipsLeft--;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
	}
}

void sendCommandLineKeys(State *state, int32_t c)
{
	if (c == 27) { // ESC
		backspaceAll(&state->commandLine);
		state->mode = NORMAL;
	} else if (' ' <= c && c <= '~') {
		add(&state->commandLine, c);
	} else if (c == ctrl('t')) {
		add(&state->commandLine, '\t');
	} else if (c == KEY_LEFT) {
		moveCursorLeft(&state->commandLine);
	} else if (c == KEY_RIGHT) {
		moveCursorRight(&state->commandLine);
	} else if (c == ctrl('g')) {
		if (state->commandLine.query.length() > 0) {
			if (state->commandLine.query[0] == 'g') {
				state->commandLine.query = state->commandLine.query.substr(1);
				if (state->commandLine.cursor > 0) {
					state->commandLine.cursor -= 1;
				}
			} else {
				state->commandLine.query = 'g' + state->commandLine.query;
				state->commandLine.cursor += 1;
			}
		}
	} else if (c == ctrl('r')) {
		struct searchReplace temp = getSearchReplace(state->commandLine.query);
		add(&state->commandLine, temp.first);
	} else if (c == KEY_BTAB) {
		autocompleteCommandLinePath(state, true);
	} else if (c == ctrl('i')) {
		autocompleteCommandLinePath(state, false);
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->commandLine.query.length() > 1 && safeSubstring(state->commandLine.query, 0, 2) == "e ") {
			struct DirSplit dirSplit = getCurrentDirSplit(state, state->commandLine.query.substr(2));
			if (dirSplit.currentUncompleted.length() == 0) {
				bool found = false;
				int32_t num = 0;
				for (int32_t i = state->commandLine.query.length() - 1; i > 0; i--) {
					if (num != 0 && state->commandLine.query[i] == '/') {
						for (int32_t j = 0; j < num; j++) {
							backspace(&state->commandLine);
						}
						found = true;
						break;
					}
					num++;
				}
				if (!found) {
					backspace(&state->commandLine);
				}
			} else {
				backspace(&state->commandLine);
			}
		} else {
			backspace(&state->commandLine);
		}
	} else if (c == ctrl('w')) {
		backspaceWord(&state->commandLine);
	} else if (c == ctrl('l')) {
		backspaceAll(&state->commandLine);
	} else if (c == ctrl('v')) {
		add(&state->commandLine, getFromClipboard(state, true));
	} else if (c == '\n') {
		evaluateCommandLineQuery(state);
		backspaceAll(&state->commandLine);
		state->mode = NORMAL;
	}
	if (c != ctrl('i') && c != KEY_BTAB) {
		state->commandLineState = { 0, "", "" };
	}
}
