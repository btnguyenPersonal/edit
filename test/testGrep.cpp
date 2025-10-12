#include <iostream>
#include "../src/util/helper.h"

int main()
{
	std::vector<grepMatch> v = grepFiles(std::filesystem::path("../vscode"), "a", false);
	for (uint32_t i = 0; i < v.size(); i++) {
		std::cout << v[i].path.string() << ":" << v[i].lineNum << std::endl;
	}
}
