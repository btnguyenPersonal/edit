#include "../util/state.h"
#include "../util/cleanup.h"
#include "../keybinds/sendKeys.h"
#include <iostream>
#include <random>
#include <vector>
#include <chrono>

std::string getEscapedChar(char c)
{
	switch (c) {
	case '\x00':
		return "C-@";
	case '\x01':
		return "C-A";
	case '\x02':
		return "C-B";
	case '\x03':
		return "C-C";
	case '\x04':
		return "C-D";
	case '\x05':
		return "C-E";
	case '\x06':
		return "C-F";
	case '\x07':
		return "C-G";
	case '\x08':
		return "C-H";
	case '\x09':
		return "C-I";
	case '\x0A':
		return "C-J";
	case '\x0B':
		return "C-K";
	case '\x0C':
		return "C-L";
	case '\x0D':
		return "C-M";
	case '\x0E':
		return "C-N";
	case '\x0F':
		return "C-O";
	case '\x10':
		return "C-P";
	case '\x11':
		return "C-Q";
	case '\x12':
		return "C-R";
	case '\x13':
		return "C-S";
	case '\x14':
		return "C-T";
	case '\x15':
		return "C-U";
	case '\x16':
		return "C-V";
	case '\x17':
		return "C-W";
	case '\x18':
		return "C-X";
	case '\x19':
		return "C-Y";
	case '\x1A':
		return "C-Z";
	case '\x1B':
		return "Esc";
	case '\x1C':
		return "C-\\";
	case '\x1D':
		return "C-]";
	case '\x1E':
		return "C-^";
	case '\x1F':
		return "C-_";
	case '\x7F':
		return "Del";
	case '\\':
		return " \\\\";
	default:
		return std::string("  ") + c;
	}
}

std::string getMode(uint32_t mode) {
	switch(mode) {
		case 0: return "VISUAL";
		case 1: return "COMMANDLINE";
		case 2: return "TYPING";
		case 3: return "SHORTCUTS";
		case 4: return "FINDFILE";
		case 5: return "GREP";
		case 6: return "BLAME";
		case 7: return "MULTICURSOR";
		case 8: return "SEARCH";
		case 9: return "FILEEXPLORER";
		case 10: return "NAMING";
		default: return "";
	}
}

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
	State *state = new State("src/util/helper.cpp");
	state->dontSave = true;

	auto start = std::chrono::high_resolution_clock::now();

	try {
		std::cout << "Test #" << testnum << ":\n";
		for (int i = 0; i < iterations; i++) {
			char randomKey = keypresses[dis(gen)];

			std::cout << getMode(state->mode) << "  " << getEscapedChar(randomKey) << std::endl;
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
		}
	} catch (const std::exception &e) {
		exit(1);
		std::cerr << "Error during fuzzing: " << e.what();
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << duration.count() << "ms" << std::endl;
}

int main()
{
	for (int i = 1; i <= 100000; i++) {
		fuzzSendKeys(i, 50);
	}
	return 0;
}
