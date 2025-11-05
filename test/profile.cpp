#include <iostream>
#include "../src/util/helper.h"
#include "../src/util/find.h"
#include "../src/util/grep.h"
#include "../src/util/history.h"
#include "../src/util/rope.h"
#include "../src/keybinds/sendKeys.h"
#include "../src/util/cleanup.h"

#include <chrono>
#include <thread>

void diff() {
	std::vector<std::string> a = {
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"17",
		"18",
		"19",
		"20",
		"21",
		"22",
	};
	std::vector<std::string> b = {
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14a",
		"zzzzz",
		"15",
		"16",
		"17",
		"18",
		"19",
		"20",
		"21",
		"22",
	};
	Rope aa;
	Rope bb;
	for (uint32_t j = 0; j < 1; j++) {
		for (uint32_t i = 0; i < a.size(); i++) {
			aa.push_back(a[i]);
		}
		for (uint32_t i = 0; i < b.size(); i++) {
			bb.push_back(b[i]);
		}
	}
	std::vector<diffLine> diff = generateDiff(aa, bb);
	for (uint32_t i = 0; i < diff.size(); i++) {
		std::cout << (diff[i].add ? "+" : "-") << " " << diff[i].lineNum << " " << diff[i].line << std::endl;
	}
	std::cout << "###################" << bb[14] << std::endl;
	State* state = new State("./test-file.h");
	// state->data = bb;
	// sendKeys(state, ':');
	// sendKeys(state, '1');
	// sendKeys(state, '4');
	// sendKeys(state, ctrl('m'));
	std::vector<char> cs = {'j', 'j', 'j', 'v', '$', 'r', 'z'};
	for (uint32_t i = 0; i < cs.size(); i++) {
		sendKeys(state, cs[i]);
		cleanup(state, cs[i]);
	}
	for (uint32_t i = 0; i < state->data.size(); i++) {
		std::cout << "data i: " << i << " " << state->data[i] << std::endl;
	}
	for (uint32_t i = 0; i < state->previousState.size(); i++) {
		std::cout << "prev i: " << i << " " << state->previousState[i] << std::endl;
	}
	// sendKeys(state, 'd');
	// cleanup(state, 'd');
	// sendKeys(state, 'd');
	// cleanup(state, 'd');
	// for (uint32_t i = 0; i < state->data.size(); i++) {
	// 	std::cout << "data i: " << i << " " << state->data[i] << std::endl;
	// }
	// for (uint32_t i = 0; i < state->previousState.size(); i++) {
	// 	std::cout << "prev i: " << i << " " << state->previousState[i] << std::endl;
	// }
	// sendKeys(state, 'u');
	// cleanup(state, 'u');
	// sendKeys(state, 'u');
	// cleanup(state, 'u');
	// std::cout << std::endl;
	// applyDiff(state, diff, true);
	// for (uint32_t i = 0; i < state->data.size(); i++) {
	// 	std::cout << "i: " << i << " " << state->data[i] << std::endl;
	// }
	// std::cout << std::endl;
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
	// Rope* rope = new Rope();
	// for (uint32_t i = 0; i < 500; i++) {
	// 	rope->push_back(std::string("hi ") + std::to_string(i));
	// }
	// Rope* ropeCopy = rope->copy();
	// for (uint32_t i = 0; i < ropeCopy->size(); i++) {
	// 	ropeCopy->erase(i);
	// }
	// for (uint32_t i = 0; i < rope->size(); i++) {
	// 	std::cout << (*rope)[i] << std::endl;
	// }
	// std::cout << "#########################################" << std::endl;
	// for (uint32_t i = 0; i < ropeCopy->size(); i++) {
	// 	std::cout << (*ropeCopy)[i] << std::endl;
	// }
	diff();
}
