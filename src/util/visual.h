#pragma once

#include "state.h"

bool isOpenParen(char c);
bool isCloseParen(char c);
char getCorrespondingParen(char c);
void initVisual(State *state, VisualType visualType);
void highlightRenderBounds(State *state, Bounds b);
WordPosition findQuoteBounds(const std::string &str, char quoteChar, uint32_t cursor, bool includeQuote);
WordPosition findParentheses(const std::string &str, char openParen, char closeParen, uint32_t cursor, bool includeParen);
Position matchIt(State *state);
WordPosition getBigWordPosition(const std::string &str, uint32_t cursor);
WordPosition getWordPosition(const std::string &str, uint32_t cursor);
