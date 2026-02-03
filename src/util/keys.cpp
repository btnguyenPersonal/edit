#include "keys.h"
#include <ncurses.h>
#include <string>

char getUnEscapedChar(std::string s) {
	if (s == "C-@") {
		return '\x00';
	} else if (s == "<C-A>") {
		return '\x01';
	} else if (s == "<C-B>") {
		return '\x02';
	} else if (s == "<C-C>") {
		return '\x03';
	} else if (s == "<C-D>") {
		return '\x04';
	} else if (s == "<C-E>") {
		return '\x05';
	} else if (s == "<C-F>") {
		return '\x06';
	} else if (s == "<C-G>") {
		return '\x07';
	} else if (s == "<C-H>") {
		return '\x08';
	} else if (s == "<C-I>") {
		return '\x09';
	} else if (s == "<C-J>") {
		return '\x0A';
	} else if (s == "<C-K>") {
		return '\x0B';
	} else if (s == "<C-L>") {
		return '\x0C';
	} else if (s == "<C-M>") {
		return '\x0D';
	} else if (s == "<C-N>") {
		return '\x0E';
	} else if (s == "<C-O>") {
		return '\x0F';
	} else if (s == "<C-P>") {
		return '\x10';
	} else if (s == "<C-Q>") {
		return '\x11';
	} else if (s == "<C-R>") {
		return '\x12';
	} else if (s == "<C-S>") {
		return '\x13';
	} else if (s == "<C-T>") {
		return '\x14';
	} else if (s == "<C-U>") {
		return '\x15';
	} else if (s == "<C-V>") {
		return '\x16';
	} else if (s == "<C-W>") {
		return '\x17';
	} else if (s == "<C-X>") {
		return '\x18';
	} else if (s == "<C-Y>") {
		return '\x19';
	} else if (s == "<C-Z>") {
		return '\x1A';
	} else if (s == "<Esc>") {
		return '\x1B';
	} else if (s == "<C-\\>") {
		return '\x1C';
	} else if (s == "<C-]>") {
		return '\x1D';
	} else if (s == "<C-^>") {
		return '\x1E';
	} else if (s == "<C-_>") {
		return '\x1F';
	} else if (s == "<Del>") {
		return '\x7F';
	} else if (s == "<Space>") {
		return ' ';
	} else if (s == " \\\\") {
		return '\\';
	} else if (s == "\\\\") {
		return '\\';
	} else {
		return s[0];
	}
}

std::string getEscapedChar(char c) {
	return getEscapedChar(c, false);
}

std::string getEscapedChar(char c, bool space) {
	switch (c) {
	case '\xFF': // KEY_MOUSE
		return "";
	case '\x00':
		return "<C-@>";
	case '\x01':
		return "<C-A>";
	case '\x02':
		return "<C-B>";
	case '\x03':
		return "<C-C>";
	case '\x04':
		return "<C-D>";
	case '\x05':
		return "<C-E>";
	case '\x06':
		return "<C-F>";
	case '\x07':
		return "<C-G>";
	case '\x08':
		return "<C-H>";
	case '\x09':
		return "<C-I>";
	case '\x0A':
		return "<C-J>";
	case '\x0B':
		return "<C-K>";
	case '\x0C':
		return "<C-L>";
	case '\x0D':
		return "<C-M>";
	case '\x0E':
		return "<C-N>";
	case '\x0F':
		return "<C-O>";
	case '\x10':
		return "<C-P>";
	case '\x11':
		return "<C-Q>";
	case '\x12':
		return "<C-R>";
	case '\x13':
		return "<C-S>";
	case '\x14':
		return "<C-T>";
	case '\x15':
		return "<C-U>";
	case '\x16':
		return "<C-V>";
	case '\x17':
		return "<C-W>";
	case '\x18':
		return "<C-X>";
	case '\x19':
		return "<C-Y>";
	case '\x1A':
		return "<C-Z>";
	case '\x1B':
		return "<Esc>";
	case '\x1C':
		return "<C-\\>";
	case '\x1D':
		return "<C-]>";
	case '\x1E':
		return "<C-^>";
	case '\x1F':
		return "<C-_>";
	case '\x7F':
		return "<Del>";
	case '"':
		return "\"";
	case ' ':
		return "<Space>";
	case '\\':
		if (space) {
			return " \\\\";
		} else {
			return "\\\\";
		}
	default:
		return std::string("") + c;
	}
}
