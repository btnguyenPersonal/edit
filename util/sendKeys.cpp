#pragma once

#include <string>
#include <vector>
#include "state.cpp"
#include "render.cpp"

void sendKeys(State* state, char c) {
    if (c == 'k') {
        state->data.push_back(std::string(1, c));
    }
}