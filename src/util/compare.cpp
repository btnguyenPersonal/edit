#include "compare.h"
#include "string.h"
#include "modes.h"
#include <cstdint>

const std::string PRE = "            ";
const std::string RES = "result";
const std::string EXP = "expect";

std::string print(std::string prefix, std::string name, std::vector<std::string> a)
{
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

struct boolWithError compare(std::vector<std::string> result, std::vector<std::string> expect)
{
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
		return { output, print(PRE, RES, result) + print(PRE, EXP, expect) };
	}
	return { output, "" };
}

std::string print(std::string prefix, std::string name, bool a)
{
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += (a == true ? "true" : "false");
	output += "\n";
	return output;
}

struct boolWithError compare(bool result, bool expect)
{
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return { output, print(PRE, RES, result) + print(PRE, EXP, expect) };
	}
	return { output, "" };
}

std::string print(std::string prefix, std::string name, std::string a)
{
	std::string output = "";
	output += prefix;
	output += name;
	output += ": \"";
	output += getPrintableString(a);
	output += "\"\n";
	return output;
}

struct boolWithError compare(std::string result, std::string expect)
{
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return { output, print(PRE, RES, result) + print(PRE, EXP, expect) };
	}
	return { output, "" };
}

std::string printInt(std::string prefix, std::string name, int32_t a)
{
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += std::to_string(a);
	output += "\n";
	return output;
}

struct boolWithError compareInt(int32_t result, int32_t expect)
{
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return { output, printInt(PRE, RES, result) + printInt(PRE, EXP, expect) };
	}
	return { output, "" };
}

std::string print(std::string prefix, std::string name, uint32_t a)
{
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += std::to_string(a);
	output += "\n";
	return output;
}

struct boolWithError compare(uint32_t result, uint32_t expect)
{
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return { output, print(PRE, RES, result) + print(PRE, EXP, expect) };
	}
	return { output, "" };
}

std::string printMode(std::string prefix, std::string name, uint32_t a)
{
	std::string output = "";
	output += prefix;
	output += name;
	output += ": ";
	output += getMode(a);
	output += "\n";
	return output;
}

struct boolWithError compare(Mode result, Mode expect)
{
	bool output = true;
	if (result != expect) {
		output = false;
	}
	if (output == false) {
		return { output, printMode(PRE, RES, result) + printMode(PRE, EXP, expect) };
	}
	return { output, "" };
}
