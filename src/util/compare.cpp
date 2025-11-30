#include "compare.h"
#include <cstdint>

void print(std::string prefix, std::vector<std::string> a, std::vector<std::string> b) {
	printf("%sa: [\n", prefix.c_str());
	for (uint32_t i = 0; i < a.size(); i++) {
		printf("%s    \"%s\",\n", prefix.c_str(), a[i].c_str());
	}
	printf("%s]\n", prefix.c_str());
	printf("%sb: [\n", prefix.c_str());
	for (uint32_t i = 0; i < b.size(); i++) {
		printf("%s    \"%s\",\n", prefix.c_str(), b[i].c_str());
	}
	printf("%s]\n", prefix.c_str());
}

bool compare(std::vector<std::string> a, std::vector<std::string> b)
{
	bool output = true;
	if (a.size() != b.size()) {
		output = false;
	} else {
		for (uint32_t i = 0; i < a.size(); i++) {
			if (a[i] != b[i]) {
				output = false;
			}
		}
	}
	if (output == false) {
		print("            ", a, b);
	}
	return output;
}
