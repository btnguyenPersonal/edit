#pragma once

#include "state.h"
#include <string>
#include <thread>
#include <atomic>
#include <vector>

void startTsServer(State *state);
void stopTsServer(State *state);
std::vector<TSError> getTSErrors(State *state);