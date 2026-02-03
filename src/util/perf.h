#pragma once

#include "state.h"
#include "timer.h"
#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

void startCheckpoint(bool debug, const std::string &name, std::vector<timer> &timers);
void printCheckpoints(bool debug, std::vector<timer> &timers);
void endLastCheckpoint(bool debug, std::vector<timer> &timers);
void clearCheckpoints(bool debug, std::vector<timer> &timers);
