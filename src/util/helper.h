#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "state.h"
#include "visualType.h"

void replaceAll(State* state, std::string query, std::string replace);
bool setSearchResultReverse(State* state);
void fixColOverMax(State* state);
WordPosition findQuoteBounds(const std::string &str, char quoteChar, uint cursor, bool includeQuote);
bool setSearchResult(State* state);
void setPosition(State* state, Position pos);
void initVisual(State* state, VisualType visualType);
bool is_number(const std::string& s);
WordPosition findParentheses(const std::string &str, char openParen, char closeParen, uint cursor, bool includeParen);
uint getIndent(const std::string& str);
uint getNextLineSameIndent(State* state);
uint getPrevLineSameIndent(State* state);
WordPosition getWordPosition(const std::string& str, uint cursor);
std::vector<grepMatch> grepFiles(const std::filesystem::path& dir_path, const std::string& query);
void generateGrepOutput(State* state);
bool isAlphaNumeric(char c);
bool filePathContainsSubstring(const std::filesystem::path& filePath, const std::string& query);
bool shouldIgnoreFile(const std::filesystem::path& path);
std::vector<std::filesystem::path> findFiles(const std::filesystem::path& dir_path, const std::string& query);
void generateFindFileOutput(State* state);
uint w(State* state);
uint b(State* state);
void insertEmptyLineBelow(State* state);
void indent(State* state);
void deindent(State* state);
void insertEmptyLine(State* state);
std::vector<std::string> readFile(std::string filename);
void saveFile(std::string filename, std::vector<std::string> data);
bool isWindowPositionInvalid(State* state);
void centerScreen(State* state);
void up(State* state);
void down(State* state);
void left(State* state);
void right(State* state);
void downHalfScreen(State* state);
void upHalfScreen(State* state);
char ctrl(char c);
void calcWindowBounds();
int getIndexFirstNonSpace(State* state);
bool handleMotion(State* state, char c, std::string motion);
bool isMotionCompleted(State* state);
void sanityCheckRowColOutOfBounds(State* state);
void sanityCheckDocumentEmpty(State* state);
int minimum(int a, int b);
int maximum(int a, int b);
