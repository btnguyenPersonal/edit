#include "history.h"
#include "state.h"
#include <algorithm>
#include <climits>
#include <string>
#include <vector>

// TODO remove
#include <iostream>

uint32_t applyDiff(State *state, std::vector<diffLine> diff, bool reverse)
{
	std::cout << "starting application" << std::endl;
	uint32_t min = UINT_MAX;
	if (reverse) {
		std::cout << "reverse" << std::endl;
		for (uint32_t i = 0; i < diff.size(); i++) {
			std::cout << "1" << std::endl;
			if (diff[i].lineNum < min) {
				std::cout << "2" << std::endl;
				min = diff[i].lineNum;
			}
			std::cout << "3" << std::endl;
			if (!diff[i].add) {
				std::cout << "4" << std::endl;
				if (diff[i].lineNum < state->data.size()) {
					std::cout << "5" << std::endl;
					std::cout << "28 diff[i].line: " << diff[i].line << std::endl;
					std::cout << "28 diff[i].lineNum: " << diff[i].lineNum << std::endl;
					state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
				} else {
					std::cout << "6" << std::endl;
					state->data.push_back(diff[i].line);
				}
			} else {
				std::cout << "7" << std::endl;
				if (diff[i].lineNum < state->data.size()) {
					std::cout << "8" << std::endl;
					std::cout << "diff[i].lineNum: " << diff[i].lineNum << std::endl;
					std::cout << "i: " << i << std::endl;
					// for (uint32_t j = 0; j < state->data.size(); j++) {
					// 	std::cout << j << " state->data: " << state->data[j] << std::endl;
					// }
					state->data.erase(state->data.begin() + diff[i].lineNum);
				}
			}
		}
	} else {
		std::cout << "noreverse" << std::endl;
		for (int32_t i = diff.size() - 1; i >= 0; i--) {
			std::cout << "a1" << std::endl;
			if (diff[i].lineNum < min) {
				std::cout << "a2" << std::endl;
				min = diff[i].lineNum;
			}
			std::cout << "a3" << std::endl;
			if (diff[i].add) {
				std::cout << "a4" << std::endl;
				if (diff[i].lineNum < state->data.size()) {
					std::cout << "a5" << std::endl;
					std::cout << "28 diff[i].line: " << diff[i].line << std::endl;
					std::cout << "28 diff[i].lineNum: " << diff[i].lineNum << std::endl;
					state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
				} else {
					std::cout << "a6" << std::endl;
					state->data.push_back(diff[i].line);
				}
			} else {
				std::cout << "a7" << std::endl;
				if (diff[i].lineNum < state->data.size()) {
					std::cout << "a8" << std::endl;
					state->data.erase(state->data.begin() + diff[i].lineNum);
				}
			}
		}
	}
	std::cout << "ending application" << std::endl;
	return min;
}

std::vector<diffLine> generateFastDiff(const std::vector<std::string> &a, const std::vector<std::string> &b)
{
	std::vector<diffLine> output;
	uint32_t aIndex = 0;
	uint32_t bIndex = 0;
	while (aIndex < a.size() || bIndex < b.size()) {
		if (bIndex >= b.size() && aIndex < a.size()) {
			output.push_back({ aIndex, false, a[aIndex] });
			aIndex++;
		} else if (aIndex >= a.size() && bIndex < b.size()) {
			output.push_back({ aIndex, true, b[bIndex] });
			bIndex++;
		} else {
			if (a[aIndex] == b[bIndex]) {
				aIndex++;
				bIndex++;
			} else if (a[aIndex] != b[bIndex]) {
				if (a.size() > b.size()) {
					output.push_back({ aIndex, false, a[aIndex] });
					aIndex++;
				} else if (b.size() > a.size()) {
					output.push_back({ aIndex, true, b[bIndex] });
					bIndex++;
				} else {
					output.push_back({ aIndex, false, a[aIndex] });
					aIndex++;
					output.push_back({ aIndex, true, b[bIndex] });
					bIndex++;
				}
			}
		}
	}
	return output;
}
