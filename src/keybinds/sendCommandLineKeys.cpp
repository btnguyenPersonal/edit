#include "sendCommandLineKeys.h"
#include "../util/clipboard.h"
#include "../util/render.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/state.h"
#include "../util/save.h"
#include <ncurses.h>
#include <sstream>
#include <string>
#include <vector>

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
		std::istringstream iss(state->commandLine.query);
		std::string s, first, second, g;
		if (std::getline(iss, s, '/') && std::getline(iss, first, '/') && std::getline(iss, second, '/')) {
			replaceCurrentLine(state, first, second);
		}
	} else if (state->commandLine.query.substr(0, 2) == "gs") {
		std::istringstream iss(state->commandLine.query);
		std::string s, first, second, g;
		if (std::getline(iss, s, '/') && std::getline(iss, first, '/') && std::getline(iss, second, '/')) {
			if (!state->dontSave) {
				renderScreen(state);
				replaceAllGlobally(state, first, second);
			}
		}
	} else if (state->commandLine.query.substr(0, 2) == "%s") {
		std::istringstream iss(state->commandLine.query);
		std::string s, first, second, g;
		do {
			if (std::getline(iss, s, '/') && std::getline(iss, first, '/') && std::getline(iss, second, '/')) {
				replaceAll(state, first, second);
			}
			std::getline(iss, s, ';');
		} while (!iss.eof());
	} else if (state->commandLine.query == "retab") {
		if (state->options.indent_style == "tab") {
			replaceAll(state, "    ", "\t");
		} else {
			replaceAll(state, "\t", std::string(state->options.indent_size, ' '));
		}
	} else if (is_number(state->commandLine.query)) {
		uint32_t number = stoul(state->commandLine.query);
		if (number > 0) {
			state->file->row = number - 1;
		} else {
			state->file->row = 0;
		}
	}
}

void autocompleteCommandLinePath(State* state, bool reverse)
{
	if (state->commandLine.query.length() > 1 && state->commandLine.query[0] == 'e') {
		std::string currentPathQuery = state->commandLine.query.substr(2);
		std::string lastDirectory = "";
		std::string currentUncompleted = "";
		for (int32_t i = currentPathQuery.length() - 1; i >= 0; i--) {
			if (currentPathQuery[i] == '/') {
				lastDirectory = currentPathQuery.substr(0, i);
				currentUncompleted = safeSubstring(currentPathQuery, i + 1);
				if (lastDirectory == "") {
					lastDirectory = "/";
				}
				break;
			}
		}
		std::vector<std::string> filesInLastDirectory;
		if (lastDirectory != "" && std::filesystem::exists(lastDirectory)) {
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
		// TODO copy gs///g query to replacement
	} else if (c == KEY_BTAB) {
		autocompleteCommandLinePath(state, true);
	} else if (c == ctrl('i')) {
		autocompleteCommandLinePath(state, false);
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->commandLine.query.length() > 0 && state->commandLine.query[0] == 'e') {
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
