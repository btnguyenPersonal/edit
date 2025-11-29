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
		state->mode = SHORTCUTS;
		std::vector<char> randVec;
		for (int i = 0; i < iterations; i++) {
			randVec.push_back(keypresses[dis(gen)]);
		}
		std::cout << "testValues({";
		for (uint32_t i = 0; i < randVec.size(); i++) {
			if (i != 0) {
				std::cout << ", ";
			}
			std::cout << "\"" << getEscapedChar(randVec[i], true) << "\"";
		}
		std::cout << "});" << std::endl;
		for (uint32_t i = 0; i < randVec.size(); i++) {
			char randomKey = randVec[i];
			std::cout << getMode(state->mode) << " " << getEscapedChar(randomKey, true) << std::endl;
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
		}
	} catch (const std::exception &e) {
		std::cerr << "Error during fuzzing: " << e.what() << std::endl;
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
			// std::cout << getMode(state->mode) << " " << getEscapedChar(randomKey, true) << std::endl;
			sendKeys(state, randomKey);
			cleanup(state, randomKey);
			ret += ",";
		}
		// initTerminal();
		// renderScreen(state);
	} catch (const std::exception &e) {
		std::cerr << "Error during fuzzing: " << e.what() << std::endl;
		std::cout << ret << std::endl;
		exit(1);
	}
}

int main()
{
	// testValues({"C-\\",";","`","C-B","I","*","A","-","-","c","r","X","H","=","C-H",">","_","Esc","}","v","P"});
	// testValues({"C-\\",";","`","C-B","I","*","A","-","-","c","r","X","H","=","C-H",">","_","Esc","}","v", "P"});
	// testValues({"=","*","}","C-B","C-S"," \\","0","E","C-M","C-@","C-]","C-V","R","d","C-]","v","`","a","`","Del","B","C-W","2","$",";","/","C-I","C-_","[","D","P","."});
	// testValues({"Q", "k", "C-I", "Esc", "C-T", "C-M", "F", "D", "C-_", "{", "f", "b", "=", "k", "S", "C-W", "C-V", "A", "C-I"});
	// testValues({"<C-H>", "<C-I>", "l", "v", "f", "<Del>", "`", "g", "r", "t", "*", "R", "J", "!", "5", "N", "v", "<C-Y>", "3", "?", "<C-Y>", "<C-U>", "<C-I>", "<C-H>", "u", "b", "*", "(", "<C-L>", "<C-_>", "\"", "Z", "<C-M>", "<C-D>", "H", "`", "K", "J", "L", "<C-R>", "x", "<C-K>", "h", "=", "+", "<C-@>", "<C-M>", "8", "=", "<C-T>"});
	// testValues({"E", "?", "<C-G>", "<C-W>", "~", "+", "(", "<C-F>", "<Del>", "*", "T", "<Space>", "#", "^", "<C-U>", "a", "<C-S>", "k", "f", "K", "f", "\"", "(", "<C-I>", "H", "+", "c", "<C-K>", "7", "<C-T>", "<Esc>", "c", "<C-N>", "~", "<C-I>", "E", "B", "%", "A", "R", "<C-M>", "X", "F", "<C-Z>", "r", "<C-I>", ">", "D", "N", "{"});
	// testValues({"<C-G>", ",", "<C-V>", "P", "I", "K", "Q", "S", "9", "<C-R>", "f", "<C-H>", "b", "<C-W>", "<C-E>", "{", "M", "+", " \\", "<C-X>", "<C-G>", "<C-M>", "7", "v", "b", "<C-G>", "]", "<C-C>", "y", "&", "!", "G", "x", "<C-R>", "<C-G>", "h", "#", "<C-E>", "!", "(", "x", "e", "!", "<C-H>", "<C-J>", "g", "x", "<C-S>", "z", "I"});
	// testValues({"E", "h", "<C-H>", "g", "?", "g", "<Esc>", "'", "l", "[", "(", ";", "'", "%", "`", "8", "Y", "v", "?", "<C-Z>", "#", "=", "<C-B>", "S", "#", "_", "5", "@", "h", "O", "7", "<C-J>", "E", "m", "3", "P", "p", "<C-M>", "<C-K>", "s", "<Esc>", " \\", "Z", "e", "<C-Z>", "!", "<C-B>", "<C-Y>", "M", "#"});
	// testValues({"i", "X", "x", "<C-H>", "T", "E", "Q", "<C-X>", ")", "d", "<C-A>", "+", "<C-D>", "k", "<C-U>", "<C-J>", "Y", "v", "<C-U>", "<C-V>", "<C-^>", "|", "K", "<C-T>", "Z", "5", "7", "<C-K>", "V", "<C-P>", "<C-@>", "_", "j", "Z", ",", "<Esc>", "l", "b", "+", "p", "<C-U>", "<C-G>", "z", "<C-V>", ">", "<C-Y>", "u", "m", "G", "*"});
	// testValues({"<C-R>", "{", "<C-P>", "<C-N>", "F", "[", "<C-K>", "<C-A>", "-", "<C-I>", "q", "<C-U>", "<C-K>", "6", "?", "~", "6", "e", "w", "<C-_>", "U", "b", "<Esc>", "<C-A>", "(", "<Del>", "<C-U>", "<C-H>", "{", "!", "J", "<C-]>", "<C-H>", "<Space>", "^", "L", "<C-O>", "<C-_>", "!", "v", "<C-H>", "/", "6", "k", "<C-H>", ",", "C", "0", "6", "x"});
	// testValues({"?", "<C-R>", "1", "]", "]", "<C-J>", "<Space>", "<C-^>", ":", "x", "<C-J>", "<C-X>", "<C-H>", "<C-_>", "^", ",", "<C-A>", "Q", ",", "W", "z", "<C-Z>", "|", "<C-J>", "#", "<C-X>", "<C-@>", "<C-E>", "h", "g", "^", "+", "i", "<C-J>", "<C-E>", "W", " \\", "H", "<C-T>", "<C-M>", "i", "E", "m", "p", "<C-Y>", "h", "^", "G", "B", "b"});
	for (int i = 1; i <= 100000; i++) {
		testValues({ "8",	"i",	   "b",	    "0",     "<C-W>",	".",	 "f",	  "A",	   "Q",
			     "<C-G>",	"<C-]>",   "b",	    "^",     "o",	"O",	 "q",	  "#",	   "u",
			     "a",	"z",	   "<C-W>", "<C-Q>", "x",	"<C-U>", "C",	  "<C-@>", "<C-K>",
			     "%",	"X",	   "O",	    "Y",     "2",	"c",	 "Q",	  "<C-L>", "E",
			     "<C-Y>",	"q",	   "<C-J>", "<C-J>", "Z",	"3",	 "<C-D>", ":",	   "<C-U>",
			     "?",	",",	   "&",	    "-",     "P",	">",	 "W",	  "<C-L>", "<C-P>",
			     "S",	"%",	   " \\",   "t",     "1",	"J",	 "c",	  "\"",	   "d",
			     "<C-G>",	"s",	   "9",	    "n",     "<C-Q>",	"Y",	 "%",	  "Z",	   "T",
			     "]",	"(",	   "R",	    "<C-N>", "\"",	"M",	 "<C-Q>", "~",	   ",",
			     "<Esc>",	"K",	   "<C-R>", "G",     "<C-\>",	";",	 "?",	  "<C-U>", "x",
			     "w",	"u",	   "-",	    "I",     ";",	"P",	 "s",	  "<C-@>", "H",
			     "]",	"o",	   "/",	    "J",     "K",	"<C-@>", "0",	  "d",	   "2",
			     "8",	"+",	   "s",	    "<C-I>", "V",	"m",	 "T",	  "<C-Z>", "<C-A>",
			     "8",	"v",	   "i",	    "2",     "x",	"x",	 "{",	  "x",	   "<C-N>",
			     "d",	"C",	   "f",	    "z",     "`",	"<C-\>", "2",	  "p",	   "[",
			     "n",	"<C-W>",   "1",	    "<",     ":",	"<C-_>", "'",	  "<C-M>", "s",
			     "~",	"\"",	   "e",	    "<",     "<C-B>",	"<C-K>", "5",	  "B",	   ";",
			     "Z",	"*",	   "L",	    "<C-B>", "[",	"&",	 "j",	  "<C-J>", "/",
			     "w",	"'",	   ">",	    "2",     "<C-_>",	"Y",	 "G",	  "-",	   "e",
			     "S",	"n",	   "<Esc>", "i",     "z",	"<C-U>", "[",	  "=",	   "^",
			     "l",	"<C-H>",   "b",	    "$",     "+",	"T",	 "'",	  "d",	   "w",
			     "_",	"e",	   "/",	    "m",     "<C-\>",	"`",	 "9",	  "<C-J>", "B",
			     "!",	"s",	   "<C-N>", "&",     "<C-H>",	"[",	 "<C-C>", ">",	   "w",
			     "N",	"O",	   "<C-E>", "U",     "s",	"W",	 "h",	  "p",	   "C",
			     "s",	"<C-I>",   "'",	    "1",     "<C-X>",	"(",	 ";",	  "!",	   "Q",
			     "<C-H>",	"<C-I>",   "7",	    "E",     "W",	"W",	 "y",	  "<C-Y>", "B",
			     "<C-B>",	"<C-]>",   "<C-B>", "%",     "I",	"h",	 "L",	  "O",	   "L",
			     "<C-@>",	"<C-T>",   "V",	    "<C-D>", "M",	"K",	 "j",	  "1",	   "<C-D>",
			     "C",	"<C-X>",   "W",	    "h",     "6",	"<C-D>", ".",	  "p",	   "?",
			     "g",	",",	   "L",	    "0",     "c",	"<C-I>", "4",	  "]",	   "<Space>",
			     "<C-E>",	">",	   "K",	    "<C-L>", "P",	"a",	 "<C-Y>", "d",	   "j",
			     "/",	"n",	   "W",	    "^",     "r",	"<C-D>", "<C-Q>", "d",	   "<C-G>",
			     "<C-H>",	"<C-T>",   "f",	    "1",     "H",	"K",	 "n",	  "<C-G>", "~",
			     "H",	"C",	   "H",	    "<C-U>", "v",	"G",	 "<C-I>", ".",	   "+",
			     "%",	"<C-I>",   "V",	    "3",     "U",	"w",	 "<Del>", "w",	   "z",
			     "C",	",",	   "<C-X>", "<C-Q>", " \\",	"6",	 "6",	  "<C-^>", "*",
			     "<Space>", "l",	   "O",	    "<C-A>", "V",	"+",	 "U",	  ",",	   "<C-M>",
			     "<C-U>",	"B",	   ".",	    "<C-F>", "$",	"`",	 "R",	  "U",	   "o",
			     "<C-G>",	"<Esc>",   "Q",	    "F",     "<C-J>",	"@",	 "]",	  "'",	   "f",
			     " \\",	"U",	   "d",	    "K",     "(",	"{",	 "4",	  "Q",	   "<C-P>",
			     "e",	"<C-P>",   "<C-D>", "o",     "X",	"%",	 "<C-G>", "<C-F>", "}",
			     "<C-]>",	"<C-P>",   "y",	    "O",     "`",	"<Esc>", "<C-J>", "m",	   "r",
			     "8",	"u",	   "!",	    "#",     "d",	"b",	 "T",	  "<C-O>", "L",
			     "<C-K>",	"<C-D>",   "<C-X>", "<C-]>", ";",	"<",	 " \\",	  "<C-Y>", "<C-P>",
			     "H",	"<C-H>",   "<C-A>", "W",     "<Del>",	"H",	 "q",	  "<C-C>", "<C-Y>",
			     "<C-R>",	".",	   "c",	    "<C-O>", "<C-T>",	"=",	 "9",	  "g",	   "2",
			     "?",	"z",	   "<C-Q>", "c",     "<C-R>",	"Y",	 "G",	  "t",	   "p",
			     "<C-H>",	"R",	   "J",	    "[",     "'",	"e",	 ")",	  "(",	   "I",
			     "<Esc>",	"0",	   "Y",	    "v",     "f",	"^",	 "C",	  "*",	   "#",
			     "F",	"<C-M>",   "b",	    "W",     "[",	"<C-^>", "<C-K>", "<C-F>", "x",
			     "4",	"<C-D>",   "<C-K>", "U",     "Z",	"<C-J>", "<C-D>", "?",	   ".",
			     "<C-K>",	"<Space>", "M",	    "<C-X>", "s",	"_",	 "j",	  "J",	   "O",
			     "'",	"x",	   "#",	    "{",     "<C-J>",	"<C-Q>", "<C-V>", "S",	   "{",
			     "K",	"0",	   "p",	    "Z",     "o",	"V",	 "C",	  "@",	   "<C-^>",
			     "<Space>", "<Esc>",   "<Esc>", "N",     "<Space>", "u",	 "*",	  "Q",	   "H",
			     "7",	"}",	   "<C-I>", "`",     "M" });
		// 	fuzzSendKeys(i, 500);
	}
	return 0;
}
