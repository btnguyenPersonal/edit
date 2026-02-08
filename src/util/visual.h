#pragma once

#include "state.h"

void replaceAllWithChar(State *state, int32_t c);
char changeCase(char c, bool upper, bool swap);
void changeCaseVisual(State *state, bool upper, bool swap);
void sortReverseLines(State *state);
void sortLines(State *state);
void surroundParagraph(State *state, bool includeLastLine);
Position changeInVisual(State *state);
void swapChars(std::vector<int32_t> &v, int32_t x, int32_t y);
void logDotCommand(State *state);
std::string getInVisual(State *state, bool addNewlines);
bool isValidMoveableChunk(State *state, Bounds bounds);
Bounds getBounds(State *state);
void setStateFromWordPosition(State *state, WordPosition pos);
std::string getInVisual(State *state);
Position changeInVisual(State *state);
Position copyInVisualSystem(State *state);
Position copyInVisual(State *state);
Position deleteInVisual(State *state);
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
