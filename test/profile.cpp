#include "../src/keybinds/sendFindKeys.h"
#include "../src/util/cleanup.h"
#include "../src/util/find.h"
#include "../src/util/grep.h"
#include "../src/util/history.h"
#include "../src/util/render.h"

#include <chrono>
#include <ncurses.h>
#include <thread>

void diff() {
	std::vector<std::string> a = {
	    "aacccccccccccaaa",
	    "bbbbbbbbbbbbbbbb",
	    "cccccccccccccccc",
	    "dddddddddddddddd",
	    "eeeeeeeeeeeeeeee",
	    "ffffffffffffffff",
	    "gggggggggggggggg",
	    "hhhhhhhhhhhhhhhh",
	    "iiiiiiiiiiiiiiii",
	    "jjjjjjjjjjjjjjjj",
	    "kkkkkkkkkkkkkkkk",
	    "llllllllllllllll",
	    "mmmmmmmmmmmmmmmm",
	    "nnnnnnnnnnnnnnnn",
	    "oooooooooooooooo",
	    "pppppppppppppppp",
	    "qqqqqqqqqqqqqqqq",
	    "rrrrrrrrrrrrrrrr",
	    "ssssssssssssssss",
	    "tttttttttttttttt",
	    "uuuuuuuuuuuuuuuu",
	    "vvvvvvvvvvvvvvvv",
	    "wwwwwwwwwwwwwwww",
	    "xxxxxxxxxxxxxxxx",
	    "yyyyyyyyyyyyyyyy",
	    "zzzzzzzzzzzzzzzz",
	};
	std::vector<std::string> b = {
	    "aaaaaaaaaaaaaaaa",
	    "bbbbbbbbbbbbbbbb",
	    "cccccccccccccccc",
	    "dddddddddddddddd",
	    "eeeeeeeeeeeeeeee",
	    "ffffffffffffffff",
	    "gggggggggggggggg",
	    "hhhhhhhhhhhhhhhh",
	    "iiiiiiiiiiiiiiii",
	    "jjjjjjjjjjjjjjjj",
	    "kkkkkkkkkkkkkkkk",
	    "llllllllllllllll",
	    "mmmmmmmmmmmmmmmm",
	    "nnnnnnnnnnnnnnnn",
	    "oooooooooooooooo",
	    "pppppppppppppppp",
	    "qqqqqqqqqqqqqqqq",
	    "rrrrrrrrrrrrrrrr",
	    "ssssssssssssssss",
	    "tttttttttttttttt",
	    "uuuuuuuuuuuuuuuu",
	    "vvvvvvvvvvvvvvvv",
	    "wwwwwwwwwwwwwwww",
	    "xxxxxxxxxxxxxxxx",
	    "yyyyyyyyyyyyyyyy",
	    "zzzzzzzzzzzzzzzz",
	};
	std::vector<diffLine> diff = generateDiff(a, b);
	for (uint32_t i = 0; i < diff.size(); i++) {
		printf("%s%s\n", diff[i].add ? "+ " : "- ", diff[i].line.c_str());
	}
	// applyDiff();
}

void grep() {
	std::vector<grepMatch> v = grepFiles(std::filesystem::path("."), "a", true);
}

void find() {
	std::vector<std::filesystem::path> v = find(std::filesystem::path("../vscode"), "a");
}

int main() {
	State *state = new State("./test-file.h");
	state->mode = FIND;
	generateFindOutput(state);

	sendFindKeys(state, 's');
	generateFindOutput(state);
	cleanup(state, 's');
	history(state, 's');
	renderScreen(state, false);

	sendFindKeys(state, 't');
	generateFindOutput(state);
	cleanup(state, 't');
	history(state, 't');
	renderScreen(state, false);

	sendFindKeys(state, 'a');
	generateFindOutput(state);
	cleanup(state, 'a');
	history(state, 'a');
	renderScreen(state, false);

	sendFindKeys(state, 't');
	generateFindOutput(state);
	cleanup(state, 't');
	history(state, 't');
	renderScreen(state, false);

	sendFindKeys(state, 'e');
	generateFindOutput(state);
	cleanup(state, 'e');
	history(state, 'e');
	renderScreen(state, false);
}
