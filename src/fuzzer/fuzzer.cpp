#include "../util/render.h"
#include "../util/state.h"
#include "../util/cleanup.h"
#include "../util/keys.h"
#include "../util/helper.h"
#include "../keybinds/sendKeys.h"
#include <iostream>
#include <random>
#include <vector>

void fuzzSendKeys(int testnum, int iterations = 1000)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<char> keypresses = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', '\b', '\n', '\t', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x7F', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '\'', '"', '`', '~', ';', ':', '[', ']', '{', '}', ',', '<', '.', '>', '/', '?', '|', '\\', '-', '_', '+', '=' };

	std::uniform_int_distribution<> dis(0, keypresses.size() - 1);
	system("cp test/example-file.cpp longtest.js");
	State *state = new State("longtest.js");

	std::string ret = "{";

	try {
		state->mode = NORMAL;
		std::vector<char> randVec;
		for (int i = 0; i < iterations; i++) {
			randVec.push_back(keypresses[dis(gen)]);
		}
		std::cout << "\t{" << std::endl;;
		std::cout << "\t\tState *state = new State(\"./test-file.h\", file);" << std::endl;
		std::cout << "\t\ttestValues(state, {";
		for (uint32_t i = 0; i < randVec.size(); i++) {
			if (i != 0) {
				std::cout << ", ";
			}
			std::string c = getEscapedChar(randVec[i], true);
			std::string s = "";
			for (uint32_t j = 0; j < c.length(); j++) {
				if (c[j] == '\\' || c[j] == '"') {
					s += "\\";
				}
				s += c[j];
			}
			std::cout << "\"" << s << "\"";
		}
		std::cout << "});" << std::endl;
		for (uint32_t i = 0; i < randVec.size(); i++) {
			char randomKey = randVec[i];
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
			history(state, randomKey);
		}
		std::cout << "\t\toutput.push_back({" << std::endl;
		std::cout << std::string("\t\t\t\"snapshotTest ") + std::to_string(testnum) + "\"," << std::endl;
		std::cout << "\t\t\tcompare(state->data, {";
		for (uint32_t i = 0; i < state->data.size(); i++) {
			if (i != 0) {
				std::cout << ",";
			}
			std::cout << std::endl;
			std::cout << "\t\t\t\t\"";
			std::string s = "";
			for (uint32_t j = 0; j < state->data[i].length(); j++) {
				if (state->data[i][j] == '\\' || state->data[i][j] == '"') {
					s += "\\";
				}
				s += state->data[i][j];
			}
			std::cout << s;
			std::cout << "\"";
		}
		std::cout << std::endl;
		std::cout << "\t\t\t})";
		std::cout << std::endl;
		std::cout << "\t\t});";
		std::cout << std::endl;
		std::cout << "\t}\n\n";
	} catch (const std::exception &e) {
		std::cerr << "Error during fuzzing: " << e.what() << std::endl;
		exit(1);
	}
}

int main()
{
	for (uint32_t i = 0; i < 1000; i++) {
		fuzzSendKeys(i, 10);
	}
	return 0;
}
