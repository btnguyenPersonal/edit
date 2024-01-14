#include <vector>
#include <string>
#include <climits>
#include "state.h"

uint applyDiff(State* state, const std::vector<diffLine>& diff, bool reverse) {
    uint min = UINT_MAX;
    if (reverse == false) {
        for (int i = ((int) diff.size()) - 1; i >= 0; i--) {
            if (diff[i].lineNum < min) {
                min = diff[i].lineNum;
            }
            if (diff[i].add == true) {
                state->data.erase(state->data.begin() + diff[i].lineNum);
            } else {
                state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
            }
        }
    } else if (reverse == true) {
        for (int i = 0; i < ((int) diff.size()); i++) {
            if (diff[i].lineNum < min) {
                min = diff[i].lineNum;
            }
            if (diff[i].add == true) {
                state->data.insert(state->data.begin() + diff[i].lineNum, diff[i].line);
            } else {
                state->data.erase(state->data.begin() + diff[i].lineNum);
            }
        }
    }
    // TODO check min isn't UINT_MAX?? maybe don't need
    return min;
}

std::vector<diffLine> generateDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr) {
    std::vector<diffLine> diffs;
    uint prevIndex = 0;
    uint currIndex = 0;

    // TODO make more efficient for pasting very large amounts of lines
    // pasting 40000 lines is unbearably slow to compute the diff
    // https://blog.jcoglan.com/2017/02/12/the-myers-diff-algorithm-part-1/
    // https://news.ycombinator.com/item?id=21383393
    while (prevIndex < prev.size() && currIndex < curr.size()) {
        if (prev[prevIndex] == curr[currIndex]) {
            // Lines are the same, move to next line in both vectors
            prevIndex += 1;
            currIndex += 1;
        } else {
            size_t lookaheadIndex = currIndex;

            // Look ahead in curr to find a matching line
            while (lookaheadIndex < curr.size() && prev[prevIndex] != curr[lookaheadIndex]) {
                lookaheadIndex++;
            }

            // If prev found in curr, add all lines between lookaheadIndex and curr
            if (lookaheadIndex < curr.size()) {
                while (currIndex < lookaheadIndex) {
                    diffs.push_back({currIndex, true, curr[currIndex]});
                    currIndex++;
                }
            // else if prev isn't found, delete that line
            } else {
                diffs.push_back({currIndex, false, prev[prevIndex]});
                prevIndex++;
            }
        }
    }

    // Handle remaining lines in curr (additions)
    while (currIndex < curr.size()) {
        diffs.push_back({currIndex, true, curr[currIndex]});
        currIndex += 1;
    }

    // Handle remaining lines in prev (deletions)
    while (prevIndex < prev.size()) {
        diffs.push_back({currIndex, false, prev[prevIndex]});
        prevIndex += 1;
    }

    return diffs;
}
