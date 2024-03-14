#include "history.h"
#include "state.h"
#include <climits>
#include <string>
#include <algorithm>
#include <vector>

#include <ncurses.h>
#include <iostream>

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
            if ((int) dl.lineNum + offsetNeg < static_cast<int>(state->data.size())) {
                state->data.erase(state->data.begin() + dl.lineNum + offsetNeg);
            }
            offsetNeg--;
        }
    }
    return min;
}

std::vector<diffLine> backtrack(const std::vector<std::vector<int>>& trace,
                                const std::vector<std::string>& a,
                                const std::vector<std::string>& b,
                                int max) {
    std::vector<diffLine> diff;
    // Initialize x and y at the bottom-right of the graph
    int x = a.size();
    int y = b.size();
    // Offset for handling negative k indices
    int offset = max;

    // Iterate backwards through the trace to find the path taken
    for (int d = static_cast<int>(trace.size()) - 1, k, prev_k, prev_x, prev_y; x > 0 || y > 0; --d) {
        // Calculate k based on current x and y
        k = x - y;
        // Adjusted index for v to handle negative k
        int vk = offset + k;

        // Decide the direction from which we came to the current cell
        // Check the edges first
        if (k == -d || (k != d && trace[d][vk - 1] < trace[d][vk + 1])) {
            // Came from the cell below (insertion)
            prev_k = k + 1;
        } else {
            // Came from the cell to the left (deletion)
            prev_k = k - 1;
        }

        // Calculate previous x based on the direction we came from
        prev_x = trace[d][offset + prev_k];
        // Calculate previous y based on previous x and k
        prev_y = prev_x - prev_k;

        while (x > prev_x && y > prev_y) {
            x--;
            y--;
        }
        if (x > prev_x && x != 0) {
            // x decreased but y didn't, indicating a deletion
            diff.insert(diff.begin(), diffLine{static_cast<unsigned int>(x - 1), false, a[x - 1]});
        } else if (y > prev_y && y != 0) {
            // y decreased but x didn't, indicating an insertion
            diff.insert(diff.begin(), diffLine{static_cast<unsigned int>(y - 1), true, b[y - 1]});
        }

        // Update x and y for the next iteration of the loop
        x = prev_x;
        y = prev_y;
    }
    // if (diff.size() != 0) {
    //     endwin();
    //     for (const auto& dl : diff) {
    //         std::cout << (dl.add ? "+" : "-") << " " << dl.lineNum << " " << dl.line << std::endl;
    //     }
    //     std::cout << '\n' << std::endl;
    //     exit(1);
    // }

    return diff;
}

std::vector<diffLine> generateDiff(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    // Lengths of input sequences
    int n = a.size();
    int m = b.size();
    // Maximum possible edits (all deletes followed by all adds)
    int max = n + m;
    // Main DP table
    std::vector<int> v(2 * max + 1);
    // To trace back the optimal path
    std::vector<std::vector<int>> trace;

    // Iterate through the possible edit distances d
    for (int d = 0; d <= max; ++d) {
        // For each edit distance, explore the possible "k-lines"
        for (int k = -d; k <= d; k += 2) {
            // Adjusted index for v to handle negative k
            int index = k + max;

            // Decide the starting x based on previous moves
            int x = (k == -d || (k != d && v[index - 1] < v[index + 1])) ? v[index + 1] : v[index - 1] + 1;

            // Calculate corresponding y based on x and k
            int y = x - k;

            // Follow diagonals (unchanged lines) as far as possible
            while (x < n && y < m && a[x] == b[y]) {
                x++; y++;
            }

            // Update the current state in the DP table
            v[index] = x;

            // If we've reached the end, save the state and exit loops
            if (x >= n && y >= m) {
                // Save this state for backtracking
                trace.push_back(v);
                // Exit the loops early
                goto end_loops;
            }
        }
        trace.push_back(v); // Save the state after each edit distance
    }
    end_loops:;

    // Backtrack from the saved states to find the diff
    return backtrack(trace, a, b, max);
}
