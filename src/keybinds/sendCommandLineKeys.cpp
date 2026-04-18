#include "sendCommandLineKeys.h"
#include "../util/cleanup.h"
#include "../util/clipboard.h"
#include "../util/ctrl.h"
#include "../util/defines.h"
#include "../util/external.h"
#include "../util/fileops.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/render.h"
#include "../util/save.h"
#include "../util/search.h"
#include "../util/state.h"
#include "../util/string.h"
#include "../util/switchMode.h"
#include "../util/textedit.h"
#include "../util/visual.h"
#include <ncurses.h>
#include <string>
#include <vector>

struct searchReplace {
	std::string first;
	std::string second;
};

struct searchReplace getSearchReplace(std::string query) {
	struct searchReplace output = {"", ""};
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

void evaluateCommandLineQuery(State *state) {
	if (state->commandLine.query == "q!") {
		if (!state->dontSave) {
			endwin();
			exit(0);
		}
	} else if (state->commandLine.query == "a") {
		state->options.autosave = !state->options.autosave;
	} else if (state->commandLine.query == "q") {
		if (!state->dontSave) {
			if (state->file->lastSave != state->file->historyPosition) {
				state->status = "unsaved changes for file: " + state->file->filename;
			} else {
				endwin();
				exit(0);
			}
		}
	} else if (state->commandLine.query == "debug") {
		state->debug = !state->debug;
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
			locateFile(state, safeSubstring(state->commandLine.query, 2), {""});
		}
	} else if (state->commandLine.query.substr(0, 1) == "s") {
		struct searchReplace temp = getSearchReplace(state->commandLine.query);
		replaceCurrentLine(state, temp.first, temp.second);
	} else if (state->commandLine.query.substr(0, 2) == "gs") {
		struct searchReplace temp = getSearchReplace(state->commandLine.query);
		if (!state->dontSave) {
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
	} else if (state->commandLine.query == "ww") {
		state->options.wordwrap = !state->options.wordwrap;
	} else if (state->commandLine.query.length() == 3 && safeSubstring(state->commandLine.query, 0, 2) == "w=" && isNumber(safeSubstring(state->commandLine.query, 2))) {
		uint32_t number = buildNumberFromString(safeSubstring(state->commandLine.query, 2));
		state->workspace = number;
	} else if (isNumber(state->commandLine.query)) {
		uint32_t number = buildNumberFromString(state->commandLine.query);
		if (number > 0) {
			state->file->row = number - 1;
		} else {
			state->file->row = 0;
		}
	} else if (state->commandLine.query == "l" || state->commandLine.query == "lint") {
		state->status = runLinter(state->file->filename);
		autoloadFile(state);
		state->skipSetHardCol = true;
		cleanup(state);
	} else if (state->commandLine.query == "blame") {
		state->file->col = 0;
		try {
			state->blame = getGitBlame(state->file->filename);
		} catch (const std::exception &e) {
			state->status = "git blame error";
		}
		state->prevMode = BLAME;
	} else if (state->commandLine.query == "diff") {
		state->logLines = getLogLines();
		state->diffLines = getDiffLines(state);
		state->viewingDiff = true;
		state->prevMode = DIFF;
	} else if (state->commandLine.query == "log") {
		state->logLines = getLogLines();
		state->viewingDiff = false;
		state->prevMode = DIFF;
	}
}

void sendCommandLineKeys(State *state, int32_t c) {
	if (c == KEY_ESCAPE) {
		backspaceAll(&state->commandLine);
		switchPrevMode(state);
	} else if (' ' <= c && c <= '~') {
		add(&state->commandLine, c);
	} else if (c == ctrl('t')) {
		add(&state->commandLine, '\t');
	} else if (c == KEY_LEFT) {
		moveCursorLeft(&state->commandLine);
	} else if (c == KEY_RIGHT) {
		moveCursorRight(&state->commandLine);
	} else if (c == ctrl('h')) {
		backspace(&state->commandLine);
	} else if (c == ctrl('w')) {
		backspaceWord(&state->commandLine);
	} else if (c == ctrl('l')) {
		backspaceAll(&state->commandLine);
	} else if (c == ctrl('v')) {
		add(&state->commandLine, getFromClipboard(state));
	} else if (c == '\n') {
		evaluateCommandLineQuery(state);
		backspaceAll(&state->commandLine);
		switchPrevMode(state);
	}
}
