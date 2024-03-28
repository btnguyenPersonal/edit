#include "history.h"
#include "state.h"
#include <algorithm>
#include <climits>
#include <string>
#include <vector>

unsigned int applyDiff(State* state, std::vector<diffLine> diff, bool reverse) {
    std::sort(diff.begin(), diff.end(), [reverse](const diffLine& a, const diffLine& b) {
        if (b.add == a.add) {
            return a.lineNum < b.lineNum;
        }
        return reverse ? b.add < a.add : a.add < b.add;
    });
    int offsetNeg = 0;
    unsigned int min = UINT_MAX;
    for (const auto& dl : diff) {
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
            if ((int)dl.lineNum + offsetNeg < static_cast<int>(state->data.size())) {
                state->data.erase(state->data.begin() + dl.lineNum + offsetNeg);
            }
            offsetNeg--;
        }
    }
    return min;
}

std::vector<diffLine> backtrack(const std::vector<std::vector<int>>& trace, const std::vector<std::string>& a, const std::vector<std::string>& b, int max) {
    std::vector<diffLine> diff;
    int x = a.size();
    int y = b.size();
    int offset = max;

    for (int d = static_cast<int>(trace.size()) - 1, k, prev_k, prev_x, prev_y; x > 0 || y > 0; --d) {
        k = x - y;
        int vk = offset + k;

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
            diff.insert(diff.begin(), diffLine{static_cast<unsigned int>(x - 1), false, a[x - 1]});
        } else if (y > prev_y && y != 0) {
            diff.insert(diff.begin(), diffLine{static_cast<unsigned int>(y - 1), true, b[y - 1]});
        }

        x = prev_x;
        y = prev_y;
    }

    return diff;
}

std::vector<diffLine> generateDiff(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    int n = a.size();
    int m = b.size();
    int max = n + m;
    std::vector<int> v(2 * max + 1);
    std::vector<std::vector<int>> trace;

    for (int d = 0; d <= max; ++d) {
        for (int k = -d; k <= d; k += 2) {
            int index = k + max;

            int x = (k == -d || (k != d && v[index - 1] < v[index + 1])) ? v[index + 1] : v[index - 1] + 1;

            int y = x - k;

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
