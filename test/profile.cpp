#include <iostream>
#include "../src/util/helper.h"
#include "../src/util/find.h"
#include "../src/util/grep.h"
#include "../src/util/history.h"

#include <chrono>
#include <thread>

void diff()
{
	std::vector<std::string> a = {
		"aacccccccccccaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc", "dddddddddddddddd", "eeeeeeeeeeeeeeee", "ffffffffffffffff", "gggggggggggggggg", "hhhhhhhhhhhhhhhh", "iiiiiiiiiiiiiiii", "jjjjjjjjjjjjjjjj", "kkkkkkkkkkkkkkkk", "llllllllllllllll", "mmmmmmmmmmmmmmmm", "nnnnnnnnnnnnnnnn", "oooooooooooooooo", "pppppppppppppppp", "qqqqqqqqqqqqqqqq", "rrrrrrrrrrrrrrrr", "ssssssssssssssss", "tttttttttttttttt", "uuuuuuuuuuuuuuuu", "vvvvvvvvvvvvvvvv", "wwwwwwwwwwwwwwww", "xxxxxxxxxxxxxxxx", "yyyyyyyyyyyyyyyy", "zzzzzzzzzzzzzzzz",
	};
	std::vector<std::string> b = {
		"aaaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc", "dddddddddddddddd", "eeeeeeeeeeeeeeee", "ffffffffffffffff", "gggggggggggggggg", "hhhhhhhhhhhhhhhh", "iiiiiiiiiiiiiiii", "jjjjjjjjjjjjjjjj", "kkkkkkkkkkkkkkkk", "llllllllllllllll", "mmmmmmmmmmmmmmmm", "nnnnnnnnnnnnnnnn", "oooooooooooooooo", "pppppppppppppppp", "qqqqqqqqqqqqqqqq", "rrrrrrrrrrrrrrrr", "ssssssssssssssss", "tttttttttttttttt", "uuuuuuuuuuuuuuuu", "vvvvvvvvvvvvvvvv", "wwwwwwwwwwwwwwww", "xxxxxxxxxxxxxxxx", "yyyyyyyyyyyyyyyy", "zzzzzzzzzzzzzzzz",
	};
	std::vector<diffLine> diff = generateFastDiff(a, b);
	for (uint32_t i = 0; i < diff.size(); i++) {
		std::cout << std::string(diff[i].add ? "+ " : "- ") << diff[i].line << std::endl;
	}
	applyDiff();
}

void grep()
{
	std::vector<grepMatch> v = grepFiles(std::filesystem::path("../vscode"), "a", true);
	// for (uint32_t i = 0; i < v.size(); i++) {
	// 	std::cout << v[i].path.string() << ":" << v[i].lineNum << std::endl;
	// }
}

void find()
{
	std::vector<std::filesystem::path> v = find(std::filesystem::path("../vscode"), "a");
	// for (uint32_t i = 0; i < v.size(); i++) {
	// 	std::cout << v[i].string() << std::endl;
	// }
}

int main()
{
	diff();
}
