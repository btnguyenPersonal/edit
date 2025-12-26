#pragma once

#include "state.h"

void searchNextResult(State *state, bool reverse);
bool searchFromTop(State *state);
void repeatPrevLineSearch(State *state, bool reverse);
bool setSearchResultReverse(State *state, bool allowCurrent);
bool setSearchResult(State *state);
bool setSearchResultCurrentLine(State *state, const std::string &query);
void replaceAllGlobally(State *state, const std::string &query, const std::string &replace);
void replaceCurrentLine(State *state, const std::string &query, const std::string &replace);
void replaceAll(State *state, const std::string &query, const std::string &replace);
std::string getCurrentWord(State *state);
