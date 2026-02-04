#pragma once

#include "state.h"
#include <map>
#include <string>

void calcWords(File *file);
std::string autocomplete(State *state, const std::string &query);
