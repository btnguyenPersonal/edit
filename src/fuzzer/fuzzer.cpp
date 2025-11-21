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

	std::vector<char> keypresses = {
		'a',	'b',	'c',	'd',	'e',	'f',	'g',	'h',	'i',	'j',	'k',	'l',
		'm',	'n',	'o',	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',	'x',
		'y',	'z',	'A',	'B',	'C',	'D',	'E',	'F',	'G',	'H',	'I',	'J',
		'K',	'L',	'M',	'N',	'O',	'P',	'Q',	'R',	'S',	'T',	'U',	'V',
		'W',	'X',	'Y',	'Z',	'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7',
		'8',	'9',	' ',	'\b',	'\n',	'\t',	'\x00', '\x01', '\x02', '\x03', '\x04', '\x05',
		'\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11',
		'\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D',
		'\x1E', '\x1F', '\x7F', '!',	'@',	'#',	'$',	'%',	'^',	'&',	'*',	'(',
		')',	'\'',	'"',	'`',	'~',	';',	':',	'[',	']',	'{',	'}',	',',
		'<',	'.',	'>',	'/',	'?',	'|',	'\\',	'-',	'_',	'+',	'='
	};

	std::uniform_int_distribution<> dis(0, keypresses.size() - 1);
	system("cp -v src/util/helper.cpp longtest.js");
	State *state = new State("longtest.js");
	state->dontSave = true;

	std::string ret = "{";

	try {
		state->mode = NORMAL;
		std::cout << "Test #" << testnum << ":\n";
		for (int i = 0; i < iterations; i++) {
			char randomKey = keypresses[dis(gen)];

			ret += "\"";
			ret += getEscapedChar(randomKey, false);
			ret += "\"";
			std::cout << getMode(state->mode) << " " << getEscapedChar(randomKey, true) << std::endl;
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
			ret += ",";
		}
	} catch (const std::exception &e) {
		std::cerr << "Error during fuzzing: " << e.what() << std::endl;
		std::cout << ret << "}" << std::endl;
		exit(1);
	}
}

void testValues(std::vector<std::string> v)
{
	State *state = new State("src/util/helper.cpp");
	state->dontSave = true;
	std::string ret = "{";
	try {
		for (uint32_t i = 0; i < v.size(); i++) {
			char randomKey = getUnEscapedChar(v[i]);
			ret += "\"";
			ret += v[i];
			ret += "\"";
			std::cout << getMode(state->mode) << " " << getEscapedChar(randomKey, true) << std::endl;
			std::cout << "249 state->visual.col " << state->visual.col << std::endl;
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
			ret += ",";
		}
		// initTerminal();
		// renderScreen(state);
	} catch (const std::exception &e) {
		std::cerr << "Error during fuzzing: " << e.what() << std::endl;
		std::cout << ret << "}" << std::endl;
		exit(1);
	}
}

int main()
{
	// testValues({"C-\\",";","`","C-B","I","*","A","-","-","c","r","X","H","=","C-H",">","_","Esc","}","v","P"});
	// testValues({"C-\\",";","`","C-B","I","*","A","-","-","c","r","X","H","=","C-H",">","_","Esc","}","v", "P"});
	// testValues({"=","*","}","C-B","C-S"," \\","0","E","C-M","C-@","C-]","C-V","R","d","C-]","v","`","a","`","Del","B","C-W","2","$",";","/","C-I","C-_","[","D","P","."});
	// testValues({"Q", "k", "C-I", "Esc", "C-T", "C-M", "F", "D", "C-_", "{", "f", "b", "=", "k", "S", "C-W", "C-V", "A", "C-I"});
	for (int i = 1; i <= 100000000; i++) {
		fuzzSendKeys(i, 50);
	}
	// while(true) {}
	return 0;
}
