#include "prompt.h"
#include "ctrl.h"

std::string prompt(State *state, std::string startingValue)
{
	auto prevMode = state->mode;
	state->mode = NAMING;
	backspaceAll(&state->name);
	setQuery(&state->name, startingValue);
	renderScreen(state);
	int32_t c;
	while (true) {
		c = getch();
		if (c != ERR) {
			if (c == 27) { // ESC
				backspaceAll(&state->name);
				state->mode = prevMode;
				return "";
			} else if (' ' <= c && c <= '~') {
				add(&state->name, c);
			} else if (c == KEY_LEFT) {
				moveCursorLeft(&state->name);
			} else if (c == KEY_RIGHT) {
				moveCursorRight(&state->name);
			} else if (c == ctrl('h')) {
				backspace(&state->name);
			} else if (c == ctrl('l')) {
				backspaceAll(&state->name);
			} else if (c == ctrl('w')) {
				backspaceWord(&state->name);
			} else if (c == ctrl('v')) {
				add(&state->name, getFromClipboard(state, true));
			} else if (c == '\n') {
				auto name = state->name.query;
				backspaceAll(&state->name);
				state->mode = prevMode;
				return name;
			}
			renderScreen(state);
		}
	}
}
