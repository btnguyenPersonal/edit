#include "../src/util/find.h"
#include "../src/util/grep.h"
#include "../src/util/history.h"
#include "../src/util/render.h"
#include "../src/util/cleanup.h"
#include "../src/keybinds/sendGrepKeys.h"

#include <ncurses.h>
#include <chrono>
#include <thread>

void diff()
{
	std::vector<std::string> a = {
		"aacccccccccccaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc", "dddddddddddddddd", "eeeeeeeeeeeeeeee", "ffffffffffffffff", "gggggggggggggggg", "hhhhhhhhhhhhhhhh", "iiiiiiiiiiiiiiii", "jjjjjjjjjjjjjjjj", "kkkkkkkkkkkkkkkk", "llllllllllllllll", "mmmmmmmmmmmmmmmm", "nnnnnnnnnnnnnnnn", "oooooooooooooooo", "pppppppppppppppp", "qqqqqqqqqqqqqqqq", "rrrrrrrrrrrrrrrr", "ssssssssssssssss", "tttttttttttttttt", "uuuuuuuuuuuuuuuu", "vvvvvvvvvvvvvvvv", "wwwwwwwwwwwwwwww", "xxxxxxxxxxxxxxxx", "yyyyyyyyyyyyyyyy", "zzzzzzzzzzzzzzzz",
	};
	std::vector<std::string> b = {
		"aaaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc", "dddddddddddddddd", "eeeeeeeeeeeeeeee", "ffffffffffffffff", "gggggggggggggggg", "hhhhhhhhhhhhhhhh", "iiiiiiiiiiiiiiii", "jjjjjjjjjjjjjjjj", "kkkkkkkkkkkkkkkk", "llllllllllllllll", "mmmmmmmmmmmmmmmm", "nnnnnnnnnnnnnnnn", "oooooooooooooooo", "pppppppppppppppp", "qqqqqqqqqqqqqqqq", "rrrrrrrrrrrrrrrr", "ssssssssssssssss", "tttttttttttttttt", "uuuuuuuuuuuuuuuu", "vvvvvvvvvvvvvvvv", "wwwwwwwwwwwwwwww", "xxxxxxxxxxxxxxxx", "yyyyyyyyyyyyyyyy", "zzzzzzzzzzzzzzzz",
	};
	std::vector<diffLine> diff = generateDiff(a, b);
	for (uint32_t i = 0; i < diff.size(); i++) {
		printf("%s%s\n", diff[i].add ? "+ " : "- ", diff[i].line.c_str());
	}
	// applyDiff();
}

void grep()
{
	std::vector<grepMatch> v = grepFiles(std::filesystem::path("."), "a", true);
}

void find()
{
	std::vector<std::filesystem::path> v = find(std::filesystem::path("../vscode"), "a");
}

int main()
{
	State *state = new State("./test-file.h");
	state->mode = GREP;
	generateGrepOutput(state, false);

	sendGrepKeys(state, 's');
	generateGrepOutput(state, false);
	cleanup(state, 's');
	history(state, 's');
	renderScreen(state, false);

	sendGrepKeys(state, 't');
	generateGrepOutput(state, false);
	cleanup(state, 't');
	history(state, 't');
	renderScreen(state, false);

	sendGrepKeys(state, 'a');
	generateGrepOutput(state, false);
	cleanup(state, 'a');
	history(state, 'a');
	renderScreen(state, false);

	sendGrepKeys(state, 't');
	generateGrepOutput(state, false);
	cleanup(state, 't');
	history(state, 't');
	renderScreen(state, false);

	sendGrepKeys(state, 'e');
	generateGrepOutput(state, false);
	cleanup(state, 'e');
	history(state, 'e');
	renderScreen(state, false);
}
