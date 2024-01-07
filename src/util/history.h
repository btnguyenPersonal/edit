#pragma once

#include <vector>
#include <string>

struct diffLine {
    uint lineNum;
    bool add; // true for add, false for delete
    std::string line;
};

void applyDiff(State* state, const std::vector<diffLine>& diff, bool reverse);
std::vector<diffLine> generateDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr);