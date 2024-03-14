#include "history.h"
#include "state.h"
#include <climits>
#include <string>
#include <algorithm>
#include <vector>

#include <ncurses.h>
#include <iostream>

// unsigned int applyDiff2(State* state, const std::vector<diffLine>& diff, bool reverse) {
//     unsigned int min = UINT_MAX;
//     if (reverse == false) {
//         for (int i = ((int)diff.size()) - 1; i >= 0; i--) {
//             if (diff[i].lineNum < min) {
//                 min = diff[i].lineNum;
//             }
//             if (diff[i].add == true) {
//                 state->data.erase(state->data.begin() + diff[i].lineNum);
//             } else {
//                 state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
//             }
//         }
//     } else if (reverse == true) {
//         for (int i = 0; i < ((int)diff.size()); i++) {
//             if (diff[i].lineNum < min) {
//                 min = diff[i].lineNum;
//             }
//             if (diff[i].add == true) {
//                 state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
//             } else {
//                 state->data.erase(state->data.begin() + diff[i].lineNum);
//             }
//         }
//     }
//     // TODO check min isn't UINT_MAX?? maybe don't need
//     return min;
// }

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
    int x = a.size(), y = b.size(); // Initialize x and y at the bottom-right of the graph
    int offset = max; // Offset for handling negative k indices

    // Iterate backwards through the trace to find the path taken
    for (int d = static_cast<int>(trace.size()) - 1, k, prev_k, prev_x, prev_y; x > 0 || y > 0; --d) {
        k = x - y; // Calculate k based on current x and y
        int vk = offset + k; // Adjusted index for v to handle negative k

        // Decide the direction from which we came to the current cell
        // Check the edges first
        if (k == -d || (k != d && trace[d][vk - 1] < trace[d][vk + 1])) {
            prev_k = k + 1; // Came from the cell below (insertion)
        } else {
            prev_k = k - 1; // Came from the cell to the left (deletion)
        }

        // Calculate previous x based on the direction we came from
        prev_x = trace[d][offset + prev_k];
        prev_y = prev_x - prev_k; // Calculate previous y based on previous x and k

        // Check the type of operation based on x and y changes
        while (x > prev_x && y > prev_y) {
            // No change in x or y implies diagonal movement (equal line), so move diagonally without adding to diff
            --x; --y;
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
    int n = a.size(), m = b.size(); // Lengths of input sequences
    int max = n + m; // Maximum possible edits (all deletes followed by all adds)
    std::vector<int> v(2 * max + 1); // Main DP table, initialized to -1 for unused states
    std::vector<std::vector<int>> trace; // To trace back the optimal path

    // Iterate through the possible edit distances d
    for (int d = 0; d <= max; ++d) {
        // For each edit distance, explore the possible "k-lines"
        for (int k = -d; k <= d; k += 2) {
            int index = k + max; // Adjusted index for v to handle negative k

            // Decide the starting x based on previous moves
            int x = (k == -d || (k != d && v[index - 1] < v[index + 1])) ? v[index + 1] : v[index - 1] + 1;

            int y = x - k; // Calculate corresponding y based on x and k

            // Follow diagonals (unchanged lines) as far as possible
            while (x < n && y < m && a[x] == b[y]) {
                x++; y++;
            }

            v[index] = x; // Update the current state in the DP table

            // If we've reached the end, save the state and exit loops
            if (x >= n && y >= m) {
                trace.push_back(v); // Save this state for backtracking
                goto end_loops; // Exit the loops early
            }
        }
        trace.push_back(v); // Save the state after each edit distance
    }
    end_loops:;

    // Backtrack from the saved states to find the diff
    return backtrack(trace, a, b, max);
}

// std::vector<diffLine> generateDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr) {
//     std::vector<diffLine> diffs;
//     unsigned int prevIndex = 0;
//     unsigned int currIndex = 0;

//     // TODO make more efficient for pasting very large amounts of lines
//     // pasting 40000 lines is unbearably slow to compute the diff
//     // https://blog.jcoglan.com/2017/02/12/the-myers-diff-algorithm-part-1/
//     // https://news.ycombinator.com/item?id=21383393
//     // https://chat.openai.com/share/997e8f1f-55cf-472f-a4da-5a4db11f92ed
//     while (prevIndex < prev.size() && currIndex < curr.size()) {
//         if (prev[prevIndex] == curr[currIndex]) {
//             // Lines are the same, move to next line in both vectors
//             prevIndex += 1;
//             currIndex += 1;
//         } else {
//             size_t lookaheadIndex = currIndex;

//             // Look ahead in curr to find a matching line
//             while (lookaheadIndex < curr.size() && prev[prevIndex] != curr[lookaheadIndex]) {
//                 lookaheadIndex++;
//             }

//             // If prev found in curr, add all lines between lookaheadIndex and curr
//             if (lookaheadIndex < curr.size()) {
//                 while (currIndex < lookaheadIndex) {
//                     diffs.push_back({currIndex, true, curr[currIndex]});
//                     currIndex++;
//                 }
//                 // else if prev isn't found, delete that line
//             } else {
//                 diffs.push_back({currIndex, false, prev[prevIndex]});
//                 prevIndex++;
//             }
//         }
//     }

//     // Handle remaining lines in curr (additions)
//     while (currIndex < curr.size()) {
//         diffs.push_back({currIndex, true, curr[currIndex]});
//         currIndex += 1;
//     }

//     // Handle remaining lines in prev (deletions)
//     while (prevIndex < prev.size()) {
//         diffs.push_back({currIndex, false, prev[prevIndex]});
//         prevIndex += 1;
//     }

//     return diffs;
// }
