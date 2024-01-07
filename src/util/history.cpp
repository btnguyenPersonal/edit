#include <vector>
#include <string>

struct diffLine {
    uint lineNum;
    bool add; // true for add, false for delete
    std::string line;
};

std::vector<diffLine> generateDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr) {
    std::vector<diffLine> diffs;
    uint prevIndex = 0;
    uint currIndex = 0;

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

            // If prev found in curr, add all all lines inbetween found line and curr
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
