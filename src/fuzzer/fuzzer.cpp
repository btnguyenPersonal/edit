#include "../util/render.h"
#include "../util/state.h"
#include "../util/cleanup.h"
#include "../util/keys.h"
#include "../keybinds/sendKeys.h"
#include <random>
#include <vector>

void fuzzSendKeys(int testnum, int iterations = 1000)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<char> keypresses = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', '\b', '\n', '\t', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x7F', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '\'', '"', '`', '~', ';', ':', '[', ']', '{', '}', ',', '<', '.', '>', '/', '?', '|', '\\', '-', '_', '+', '=' };

	std::uniform_int_distribution<> dis(0, keypresses.size() - 1);
	State *state = new State("src/util/helper.cpp");

	std::string ret = "{";

	try {
		std::vector<char> randVec;
		for (int i = 0; i < iterations; i++) {
			randVec.push_back(keypresses[dis(gen)]);
		}
		printf("State *state = new State(\"src/helper.cpp\");\n");
		printf("testValues(state, {");
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
			char randomKey = randVec[i];
			printf("%s %s\n", getMode(state->mode).c_str(), getEscapedChar(randomKey, true).c_str());
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
			history(state, randomKey);
		}
	} catch (const std::exception &e) {
		printf("Error during fuzzing: %s\n", e.what());
		exit(1);
	}
}

int main()
{
	for (uint32_t i = 0; i < 1000000; i++) {
		fuzzSendKeys(i, 10);
	}
	return 0;
}
