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
			if (state->lastSave != state->historyPosition) {
				state->status = "unsaved changes for file: " + state->filename;
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
			state->row = number - 1;
		} else {
			state->row = 0;
		}
	} else {
		state->readConfigLine(state->commandLine.query);
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
	} else if (c == KEY_BACKSPACE || c == 127) {
		backspace(&state->commandLine);
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
}
