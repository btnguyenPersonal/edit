#pragma once

#include "state.h"
#include "visualType.h"
#include <filesystem>
#include <string>
#include <vector>

std::string replace(std::string str, const std::string &from, const std::string &to);
uint32_t buildNumberFromString(std::string s);
std::string runCommand(std::string command);
std::vector<std::string> splitByChar(std::string text, char c);
uint32_t isLargeFile(State *state);
uint32_t getLastCharIndex(State *state);
void realignHarpoon(State *state);
std::vector<std::string> getDiffLines(State *state);
std::vector<std::string> getLogLines(State *state);
uint32_t getNormalizedCol(State *state, uint32_t hardCol);
bool isColTooSmall(State *state);
bool isWhitespace(char c);
bool isColTooBig(State *state);
uint32_t getDisplayCol(State *state);
uint32_t getDisplayLength(State *state, std::string s);
std::string padTo(std::string str, const uint32_t num, const char paddingChar);
void forwardFileStack(State *state);
void backwardFileStack(State *state);
void swapCase(State *state, uint32_t r, uint32_t c);
void focusHarpoon(State *state);
void recordHistory(State *state, std::vector<diffLine> diff);
void recordJumpList(State *state);
void recordMacroCommand(State *state, char c);
void insertFinalEmptyNewline(State *state);
char getIndentCharacter(State *state);
uint32_t getIndentSize(State *state);
uint32_t getIndent(State *state, const std::string &str);
void recordMotion(State *state, int32_t c);
void setDotCommand(State *state, int32_t c);
void setDotCommand(State *state, std::vector<int32_t> s);
bool eraseHarpoon(State *state);
bool containsHarpoon(State *state);
void clearHarpoon(State *state);
bool createNewestHarpoon(State *state);
void jumpToPrevHarpoon(State *state);
void jumpToNextHarpoon(State *state);
bool jumpToHarpoon(State *state, uint32_t num);
bool isAlphanumeric(char c);
bool isOffScreenVertical(State *state);
uint32_t getCenteredWindowPosition(State *state);
uint32_t getDisplayRows(State *state, uint32_t r, uint32_t c);
uint32_t getDisplayRows(State *state, uint32_t r);
void refocusFileExplorer(State *state, bool changeMode);
std::string setStringToLength(const std::string &s, uint32_t length, bool showTilde);
bool isInt(const std::string &s);
void changeToGrepFile(State *state);
void findDefinitionFromGrepOutput(State *state, std::string s);
void centerFileExplorer(State *state);
void resetValidCursorState(State *state);
void getAndAddNumber(State *state, uint32_t row, uint32_t col, int32_t num);
std::string safeSubstring(const std::string &str, std::size_t pos, std::size_t len);
std::string safeSubstring(const std::string &str, std::size_t pos);
std::string getGitHash(State *state);
std::vector<std::string> getGitBlame(const std::string &filename);
uint32_t getLineNumberOffset(State *state);
std::string getCommentSymbol(const std::string &filename);
void moveHarpoonLeft(State *state);
void moveHarpoonRight(State *state);
bool isWindowPositionHorizontalInvalid(State *state);
void ltrim(std::string &s);
void rtrim(std::string &s);
void setPosition(State *state, Position pos);
bool isNumber(std::string s);
uint32_t getIndent(const std::string &str);
void insertEmptyLineBelow(State *state);
void indent(State *state);
void deindent(State *state);
void insertEmptyLine(State *state);
void trimTrailingWhitespace(State *state);
bool isWindowPositionInvalid(State *state);
void centerScreen(State *state);
char ctrl(char c);
char unctrl(char c);
void calcWindowBounds();
int32_t getIndexFirstNonSpace(State *state);
int32_t minimum(int32_t a, int32_t b);
int32_t maximum(int32_t a, int32_t b);
