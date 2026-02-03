#include "compare.h"
#include "modes.h"
#include "string.h"
#include <cstdint>

const std::string PRE = "            ";
const std::string RES = "result";
const std::string EXP = "expect";

std::string getPrintColor(int32_t color) {
	if (color == BLACK) {
		return "BLACK";
	} else if (color == GREY) {
		return "GREY";
	} else if (color == RED) {
		return "RED";
	} else if (color == GREEN) {
		return "GREEN";
	} else if (color == YELLOW) {
		return "YELLOW";
	} else if (color == BLUE) {
		return "BLUE";
	} else if (color == MAGENTA) {
		return "MAGENTA";
	} else if (color == CYAN) {
		return "CYAN";
	} else if (color == WHITE) {
		return "WHITE";
	} else if (color == DIFFGREEN) {
		return "DIFFGREEN";
	} else if (color == DIFFRED) {
		return "DIFFRED";
	} else if (color == invertColor(BLACK)) {
		return "invertColor(BLACK)";
	} else if (color == invertColor(GREY)) {
		return "invertColor(GREY)";
	} else if (color == invertColor(RED)) {
		return "invertColor(RED)";
	} else if (color == invertColor(GREEN)) {
		return "invertColor(GREEN)";
	} else if (color == invertColor(YELLOW)) {
		return "invertColor(YELLOW)";
	} else if (color == invertColor(BLUE)) {
		return "invertColor(BLUE)";
	} else if (color == invertColor(MAGENTA)) {
		return "invertColor(MAGENTA)";
	} else if (color == invertColor(CYAN)) {
		return "invertColor(CYAN)";
	} else if (color == invertColor(WHITE)) {
		return "invertColor(WHITE)";
	} else if (color == invertColor(DIFFGREEN)) {
		return "invertColor(DIFFGREEN)";
	} else if (color == invertColor(DIFFRED)) {
		return "invertColor(DIFFRED)";
	} else {
		return "ERROR COLOR NOT FOUND";
	}
}

std::string print(std::string prefix, std::string name, std::vector<PixelPos> a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": [\n";
	for (uint32_t i = 0; i < a.size(); i++) {
		output += prefix;
		output += "    {\n";
		output += prefix;
		output += "         pixel: " + std::string("{ ") + (char)(a[i].pixel.c & A_CHARTEXT) + ", " + getPrintColor(a[i].pixel.color) + " },\n";
		output += prefix;
		output += "         r: " + std::to_string(a[i].r) + ",\n";
		output += prefix;
		output += "         c: " + std::to_string(a[i].c) + ",\n";
		output += prefix;
		output += "    },\n";
	}
	output += prefix;
	output += "]\n";
	return output;
}

struct boolWithError compare(std::vector<PixelPos> result, std::vector<PixelPos> expect) {
	bool output = true;
	if (result.size() != expect.size()) {
		output = false;
	} else {
		for (uint32_t i = 0; i < result.size(); i++) {
			bool pixelDifferent = result[i].pixel.c != expect[i].pixel.c || result[i].pixel.color != expect[i].pixel.color;
			bool locationDifferent = result[i].r != expect[i].r || result[i].c != expect[i].c;
			if (pixelDifferent || locationDifferent) {
				output = false;
				break;
			}
		}
	}
	if (output == false) {
		return {output, print(PRE, RES, result) + print(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string print(std::string prefix, std::string name, std::vector<Pixel> a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": [\n";
	for (uint32_t i = 0; i < a.size(); i++) {
		output += prefix;
		output += "    ";
		output += std::string("{ ") + (char)(a[i].c & A_CHARTEXT) + ", " + getPrintColor(a[i].color) + "},";
		output += "\n";
	}
	output += prefix;
	output += "]\n";
	return output;
}

struct boolWithError compare(std::vector<Pixel> result, std::vector<Pixel> expect) {
	bool output = true;
	if (result.size() != expect.size()) {
		output = false;
	} else {
		for (uint32_t i = 0; i < result.size(); i++) {
			if (result[i].c != expect[i].c || result[i].color != expect[i].color) {
				output = false;
				break;
			}
		}
	}
	if (output == false) {
		return {output, print(PRE, RES, result) + print(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string print(std::string prefix, std::string name, std::vector<std::string> a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": [\n";
	for (uint32_t i = 0; i < a.size(); i++) {
		output += prefix;
		output += "    \"";
		output += getPrintableString(a[i]);
		output += "\"\n";
	}
	output += prefix;
	output += "]\n";
	return output;
}

struct boolWithError compare(std::vector<std::string> result, std::vector<std::string> expect) {
	bool output = true;
	if (result.size() != expect.size()) {
		output = false;
	} else {
		for (uint32_t i = 0; i < result.size(); i++) {
			if (result[i] != expect[i]) {
				output = false;
				break;
			}
		}
	}
	if (output == false) {
		return {output, print(PRE, RES, result) + print(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string print(std::string prefix, std::string name, bool a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += (a == true ? "true" : "false");
	output += "\n";
	return output;
}

struct boolWithError compare(bool result, bool expect) {
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return {output, print(PRE, RES, result) + print(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string print(std::string prefix, std::string name, std::string a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": \"";
	output += getPrintableString(a);
	output += "\"\n";
	return output;
}

struct boolWithError compare(std::string result, std::string expect) {
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return {output, print(PRE, RES, result) + print(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string printInt(std::string prefix, std::string name, int32_t a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += std::to_string(a);
	output += "\n";
	return output;
}

struct boolWithError compareInt(int32_t result, int32_t expect) {
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return {output, printInt(PRE, RES, result) + printInt(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string print(std::string prefix, std::string name, uint32_t a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += std::to_string(a);
	output += "\n";
	return output;
}

struct boolWithError compare(uint32_t result, uint32_t expect) {
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return {output, print(PRE, RES, result) + print(PRE, EXP, expect)};
	}
	return {output, ""};
}

std::string printMode(std::string prefix, std::string name, uint32_t a) {
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += getMode(a);
	output += "\n";
	return output;
}

struct boolWithError compare(Mode result, Mode expect) {
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return {output, printMode(PRE, RES, result) + printMode(PRE, EXP, expect)};
	}
	return {output, ""};
}
