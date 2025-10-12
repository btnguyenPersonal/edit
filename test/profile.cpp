#include <iostream>
#include "../src/util/helper.h"
#include "../src/util/history.h"

int main()
{
	// std::vector<grepMatch> v = grepFiles(std::filesystem::path("../vscode"), "abc", true);
	// for (uint32_t i = 0; i < v.size(); i++) {
	// 	std::cout << v[i].path.string() << ":" << v[i].lineNum << std::endl;
	// }
	std::vector<std::string> a = {
		"aaaaaaaaaaaaaaaa",
		"bbbbbbbbbbbbbbbb",
		"cccccccccccccccc",
		"dddddddddddddddd",
		"eeeeeeeeeeeeeeee",
		"ffffffffffffffff",
		"gggggggggggggggg",
		"hhhhhhhhhhhhhhhh",
		"iiiiiiiiiiiiiiii",
		"jjjjjjjjjjjjjjjj",
		"kkkkkkkkkkkkkkkk",
		"llllllllllllllll",
		"mmmmmmmmmmmmmmmm",
		"nnnnnnnnnnnnnnnn",
		"oooooooooooooooo",
		"pppppppppppppppp",
		"qqqqqqqqqqqqqqqq",
		"rrrrrrrrrrrrrrrr",
		"ssssssssssssssss",
		"tttttttttttttttt",
		"uuuuuuuuuuuuuuuu",
		"vvvvvvvvvvvvvvvv",
		"wwwwwwwwwwwwwwww",
		"xxxxxxxxxxxxxxxx",
		"yyyyyyyyyyyyyyyy",
		"zzzzzzzzzzzzzzzz",
	};
	std::vector<std::string> b = {
		"zzzzzzzzzzzzzzzz",
		"yyyyyyyyyyyyyyyy",
		"xxxxxxxxxxxxxxxx",
		"wwwwwwwwwwwwwwww",
		"vvvvvvvvvvvvvvvv",
		"uuuuuuuuuuuuuuuu",
		"tttttttttttttttt",
		"ssssssssssssssss",
		"rrrrrrrrrrrrrrrr",
		"qqqqqqqqqqqqqqqq",
		"pppppppppppppppp",
		"oooooooooooooooo",
		"nnnnnnnnnnnnnnnn",
		"mmmmmmmmmmmmmmmm",
		"llllllllllllllll",
		"kkkkkkkkkkkkkkkk",
		"jjjjjjjjjjjjjjjj",
		"iiiiiiiiiiiiiiii",
		"hhhhhhhhhhhhhhhh",
		"gggggggggggggggg",
		"ffffffffffffffff",
		"eeeeeeeeeeeeeeee",
		"dddddddddddddddd",
		"cccccccccccccccc",
		"bbbbbbbbbbbbbbbb",
		"aaaaaaaaaaaaaaaa",
	};
	std::vector<std::string> aa;
	std::vector<std::string> bb;
	for (uint32_t j = 0; j < 1000; j++) {
		for (uint32_t i = 0; i < 26; i++) {
			aa.push_back(a[i]);
			aa.push_back(b[i]);
		}
	}
	std::vector<diffLine> diff = generateFastDiff(aa, bb);
	for (uint32_t i = 0; i < diff.size(); i++) {
		std::cout << diff[i].line << std::endl;
	}
}
