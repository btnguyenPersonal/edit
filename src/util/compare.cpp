#include "compare.h"
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
		output += a[i];
		output += "\"\n";
	}
	output += prefix;
	output += "]\n";
	return output;
}

struct boolWithError compare(std::vector<std::string> result, std::vector<std::string> expected)
{
	bool output = true;
	if (result.size() != expected.size()) {
		output = false;
	} else {
		for (uint32_t i = 0; i < result.size(); i++) {
			if (result[i] != expected[i]) {
				output = false;
				break;
			}
		}
	}
	if (output == false) {
		return { output, print(PRE, RES, result) + print(PRE, EXP, expected) };
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

struct boolWithError compare(bool result, bool expected)
{
	bool output = true;
	if (result != expected) {
		output = false;
	}
	if (output == false) {
		return { output, print(PRE, RES, result) + print(PRE, EXP, expected) };
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

struct boolWithError compare(uint32_t result, uint32_t expected)
{
	bool output = true;
	if (result != expected) {
		output = false;
	}
	if (output == false) {
		return { output, print(PRE, RES, result) + print(PRE, EXP, expected) };
	}
	return { output, "" };
}
