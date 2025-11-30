#include "compare.h"
#include <cstdint>

std::string print(std::string prefix, std::string name, std::vector<std::string> a) {
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

struct boolWithError compare(std::vector<std::string> a, std::vector<std::string> b)
{
	bool output = true;
	if (a.size() != b.size()) {
		output = false;
	} else {
		for (uint32_t i = 0; i < a.size(); i++) {
			if (a[i] != b[i]) {
				output = false;
				break;
			}
		}
	}
	if (output == false) {
		return { output, print("            ", "a", a) + print("            ", "b", b) };
	}
	return { output, "" };
}
