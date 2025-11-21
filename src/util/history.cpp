#include "history.h"
#include "state.h"
#include <algorithm>
#include <climits>
#include <string>
#include <vector>

uint32_t applyDiff(State *state, std::vector<diffLine> diff, bool reverse)
{
	uint32_t min = UINT_MAX;
	for (uint32_t i = 0; i < diff.size(); i++) {
		if (diff[i].lineNum < min) {
			min = diff[i].lineNum;
		}
		if ((!reverse && diff[i].add) || (reverse && !diff[i].add)) {
			if (diff[i].lineNum < state->data.size()) {
				state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
			} else {
				state->data.push_back(diff[i].line);
			}
		} else {
			if (diff[i].lineNum < state->data.size()) {
				state->data.erase(state->data.begin() + diff[i].lineNum);
			}
		}
	}
	return min;
}

std::vector<diffLine> generateFastDiff(const std::vector<std::string> &a, const std::vector<std::string> &b)
{
	std::vector<diffLine> output;
	uint32_t aIndex = 0;
	uint32_t bIndex = 0;
	while (aIndex < a.size() || bIndex < b.size()) {
		if (bIndex >= b.size()) {
			output.push_back({ aIndex, false, a[aIndex] });
			aIndex++;
		} else if (aIndex >= a.size()) {
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
					output.push_back({ aIndex, true, b[bIndex] });
					bIndex++;
					aIndex++;
				}
			}
		}
	}
	return output;
}
