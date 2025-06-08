#include "../util/render.h"
#include "../util/state.h"
#include "../util/cleanup.h"
#include "../keybinds/sendKeys.h"
#include <iostream>
#include <random>
#include <vector>
#include <chrono>

char getUnEscapedChar(std::string s)
{
	if (s == "C-@") {
		return '\x00';
	} else if (s == "C-A") {
		return '\x01';
	} else if (s == "C-B") {
		return '\x02';
	} else if (s == "C-C") {
		return '\x03';
	} else if (s == "C-D") {
		return '\x04';
	} else if (s == "C-E") {
		return '\x05';
	} else if (s == "C-F") {
		return '\x06';
	} else if (s == "C-G") {
		return '\x07';
	} else if (s == "C-H") {
		return '\x08';
	} else if (s == "C-I") {
		return '\x09';
	} else if (s == "C-J") {
		return '\x0A';
	} else if (s == "C-K") {
		return '\x0B';
	} else if (s == "C-L") {
		return '\x0C';
	} else if (s == "C-M") {
		return '\x0D';
	} else if (s == "C-N") {
		return '\x0E';
	} else if (s == "C-O") {
		return '\x0F';
	} else if (s == "C-P") {
		return '\x10';
	} else if (s == "C-Q") {
		return '\x11';
	} else if (s == "C-R") {
		return '\x12';
	} else if (s == "C-S") {
		return '\x13';
	} else if (s == "C-T") {
		return '\x14';
	} else if (s == "C-U") {
		return '\x15';
	} else if (s == "C-V") {
		return '\x16';
	} else if (s == "C-W") {
		return '\x17';
	} else if (s == "C-X") {
		return '\x18';
	} else if (s == "C-Y") {
		return '\x19';
	} else if (s == "C-Z") {
		return '\x1A';
	} else if (s == "Esc") {
		return '\x1B';
	} else if (s == "C-\\") {
		return '\x1C';
	} else if (s == "C-]") {
		return '\x1D';
	} else if (s == "C-^") {
		return '\x1E';
	} else if (s == "C-_") {
		return '\x1F';
	} else if (s == "Del") {
		return '\x7F';
	} else if (s == " \\\\") {
		return '\\';
	} else {
		return s[0];
	}
}

std::string getEscapedChar(char c, bool space)
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
		if (space) {
			return " \\\\";
		} else {
			return "\\\\";
		}
	default:
		if (space) {
			return std::string("") + c;
		} else {
			return std::string("") + c;
		}
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

	std::string ret = "{";

	try {
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

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << duration.count() << "ms" << std::endl;
}

void testValues(std::vector<std::string> v) {
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
