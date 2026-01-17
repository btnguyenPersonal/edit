#pragma once

#include "state.h"
#include <string>
#include <thread>
#include <atomic>

void startTsServer(State *state);
void stopTsServer(State *state);