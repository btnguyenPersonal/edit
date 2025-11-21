#include "history.h"
#include "state.h"
#include <algorithm>
#include <climits>
#include <string>
#include <vector>

uint32_t applyDiff(State *state, std::vector<diffLine> diff, bool reverse)
{
	std::sort(diff.begin(), diff.end(), [reverse](const diffLine &a, const diffLine &b) {
		if (b.add == a.add) {
			return a.lineNum < b.lineNum;
		}
		return reverse ? b.add < a.add : a.add < b.add;
	});
	int32_t offsetNeg = 0;
	uint32_t min = UINT_MAX;
	for (const auto &dl : diff) {
		if ((!reverse && dl.add) || (reverse && !dl.add)) {
			if (dl.lineNum < min) {
				min = dl.lineNum;
			}
			if (dl.lineNum < state->data.size()) {
				state->data.insert(state->data.begin() + dl.lineNum, dl.line);
			} else {
				state->data.push_back(dl.line);
			}
		} else {
			if (dl.lineNum + offsetNeg < min) {
				min = dl.lineNum + offsetNeg;
			}
			if ((int32_t)dl.lineNum + offsetNeg < static_cast<int32_t>(state->data.size())) {
				state->data.erase(state->data.begin() + dl.lineNum + offsetNeg);
			}
			offsetNeg--;
		}
	}
	return min;
}

std::vector<diffLine> backtrack(const std::vector<std::vector<int32_t> > &trace, const std::vector<std::string> &a,
				const std::vector<std::string> &b, int32_t max)
{
	std::vector<diffLine> diff;
	int32_t x = a.size();
	int32_t y = b.size();
	int32_t offset = max;

	for (int32_t d = static_cast<int32_t>(trace.size()) - 1, k, prev_k, prev_x, prev_y; x > 0 || y > 0; --d) {
		k = x - y;
		int32_t vk = offset + k;

		if (k == -d || (k != d && trace[d][vk - 1] < trace[d][vk + 1])) {
			prev_k = k + 1;
		} else {
			prev_k = k - 1;
		}

		prev_x = trace[d][offset + prev_k];
		prev_y = prev_x - prev_k;

		while (x > prev_x && y > prev_y) {
			x--;
			y--;
		}
		if (x > prev_x && x != 0) {
			diff.insert(diff.begin(), diffLine{ static_cast<uint32_t>(x - 1), false, a[x - 1] });
		} else if (y > prev_y && y != 0) {
			diff.insert(diff.begin(), diffLine{ static_cast<uint32_t>(y - 1), true, b[y - 1] });
		}

		x = prev_x;
		y = prev_y;
	}

	return diff;
}

std::vector<diffLine> generateDiff(const std::vector<std::string> &a, const std::vector<std::string> &b)
{
	int32_t n = a.size();
	int32_t m = b.size();
	int32_t max = n + m;
	std::vector<int32_t> v(2 * max + 1);
	std::vector<std::vector<int32_t> > trace;

	for (int32_t d = 0; d <= max; ++d) {
		for (int32_t k = -d; k <= d; k += 2) {
			int32_t index = k + max;

			int32_t x = (k == -d || (k != d && v[index - 1] < v[index + 1])) ? v[index + 1] :
											   v[index - 1] + 1;

			int32_t y = x - k;

			while (x < n && y < m && a[x] == b[y]) {
				x++;
				y++;
			}

			v[index] = x;

			if (x >= n && y >= m) {
				trace.push_back(v);
				return backtrack(trace, a, b, max);
			}
		}
		trace.push_back(v);
	}
	return backtrack(trace, a, b, max);
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
