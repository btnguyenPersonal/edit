#include "../util/state.h"
#include "../util/keys.h"
#include "../util/computeFrame.h"
#include "../util/render.h"
#include "../../test/util/debug.h"
#include <ncurses.h>
#include <random>
#include <vector>

void fuzzSendKeys(int testnum, int iterations = 1000)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<char> keypresses = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', '\b', '\n', '\t', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x7F', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '\'', '"', '`', '~', ';', ':', '[', ']', '{', '}', ',', '<', '.', '>', '/', '?', '|', '\\', '-', '_', '+', '=' };

	std::uniform_int_distribution<> dis(0, keypresses.size() - 1);
	State *state = new State("./test-file.h");
	state->dontSave = false;

	try {
		initTerminal();
		std::vector<char> randVec;
		for (int i = 0; i < iterations; i++) {
			randVec.push_back(keypresses[dis(gen)]);
		}
		printf("\t// iteration #%d\n", testnum);
		printf("\ttestValues(state, {");
		for (uint32_t i = 0; i < randVec.size(); i++) {
			if (i != 0) {
				printf(", ");
			}
			std::string c = getEscapedChar(randVec[i], true);
			std::string s = "";
			for (uint32_t j = 0; j < c.length(); j++) {
				if (c[j] == '\\' || c[j] == '"') {
					s += "\\";
				}
				s += c[j];
			}
			printf("\"%s\"", s.c_str());
		}
		printf("});\n");
		for (uint32_t i = 0; i < randVec.size(); i++) {
			char key = randVec[i];
			ungetch(key);
			computeFrame(state);
		}
		endwin();
	} catch (const std::exception &e) {
		endwin();
		printf("Error during fuzzing: %s\n", e.what());
		exit(1);
	}
}

void testValues(State *state, std::vector<std::string> v)
{
	try {
		initTerminal();
		for (uint32_t i = 0; i < v.size(); i++) {
			char key = getUnEscapedChar(v[i]);
			ungetch(key);
			computeFrame(state);
			debug(state);
		}
		endwin();
	} catch (const std::exception &e) {
		endwin();
		printf("Error during fuzzing: %s\n", e.what());
		exit(1);
	}
}

int main()
{
	State *state = new State("./test-file.h");
	// testValues(state, {"E", "N", "<C-O>", ">", ">", "l", "<C-^>", " \\\\", "3", "<C-P>"});
	// testValues(state, {"r", "~", "Q", ">", "V", "I", "p", "<C-A>", "<C-K>", "<C-@>"});
	// testValues(state, {"{", "~", "5", "r", "<Space>", "<C-O>", "<C-_>", "d", "G", "<C-]>"});
	// testValues(state, {"|", "<C-A>", "'", "<C-U>", "U", "#", "7", "T", ":", "l"});
	// testValues(state, {")", "n", "S", "b", "#", "<C-_>", ";", "c", "(", "<C-Q>"});
	// testValues(state, {"%", "[", "'", "+", "<C-E>", "}", "=", "<C-P>", "3", "X"});
	// testValues(state, {"M", "K", "p", "J", "]", "]", "T", "A", "<C-J>", "k"});
	// testValues(state, {"H", "<C-I>", "/", "<C-Q>", "'", "<C-H>", "3", "w", "2", "#"});
	// testValues(state, {"<C-G>", "Z", "<C-O>", "<C-P>", "p", "<C-@>", "1", "<C-J>", "M", "5"});
	// testValues(state, {"z", "v", "<C-@>", "<C-H>", "T", "*", ".", "2", "=", "."});
	// testValues(state, {"V", "=", ".", "<C-P>", "<C-]>", "U", "<Esc>", "Y", "h", "p"});
	// testValues(state, {"l", "<C-I>", "#", "h", "<C-I>", "<C-V>", "o", "<C-J>", "|", "d"});
	// testValues(state, {"<C-E>", "8", "<C-G>", "G", "b", "g", "J", "u", "u", "<C-L>"});
	// testValues(state, {"<C-G>", "E", "<C-V>", "<C-I>", "G", "<C-J>", "<C-O>", "$", "<C-M>", "."});
	// testValues(state, {"m", "'", "!", "d", "&", "<C-V>", "<C-E>", ":", "<C-R>", "L"});
	// testValues(state, {"w", "v", "Q", "<C-P>", "1", "&", ":", "<C-R>", "k", "j"});
	for (uint32_t i = 0; i < 1000000; i++) {
		fuzzSendKeys(i, 100);
	}
	return 0;
}
