#pragma once

#include "state.h"
#include "visualType.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

std::string safeSubstring(const std::string& str, std::size_t pos, std::size_t len);
std::string safeSubstring(const std::string& str, std::size_t pos);
bool isAllLowercase(const std::string& str);
void recordAction(State* state);
int calculateAPM(State* state);
unsigned int findNextChar(State* state, char c);
unsigned int toNextChar(State* state, char c);
unsigned int getPrevEmptyLine(State* state);
unsigned int getNextEmptyLine(State* state);
std::string getGitHash(State* state);
std::vector<std::string> getGitBlame(const std::string& filename);
unsigned int getLineNumberOffset(State* state);
void replaceAllGlobally(State* state, std::string query, std::string replace);
void replaceCurrentLine(State* state, std::string query, std::string replace);
std::string getCurrentWord(State* state);
std::string autocomplete(State* state, std::string query);
std::string getCommentSymbol(std::string filename);
std::string getExtension(std::string filename);
void moveHarpoonLeft(State* state);
void moveHarpoonRight(State* state);
bool isWindowPositionHorizontalInvalid(State* state);
void ltrim(std::string& s);
void rtrim(std::string& s);
void replaceAll(State* state, std::string query, std::string replace);
bool setSearchResultReverse(State* state);
void fixColOverMax(State* state);
WordPosition findQuoteBounds(const std::string& str, char quoteChar, unsigned int cursor, bool includeQuote);
bool setSearchResult(State* state);
void setPosition(State* state, Position pos);
void initVisual(State* state, VisualType visualType);
bool is_number(const std::string& s);
WordPosition findParentheses(const std::string& str, char openParen, char closeParen, unsigned int cursor, bool includeParen);
unsigned int getIndent(const std::string& str);
unsigned int getNextLineSameIndent(State* state);
unsigned int getPrevLineSameIndent(State* state);
WordPosition getWordPosition(const std::string& str, unsigned int cursor);
std::vector<grepMatch> grepFiles(const std::filesystem::path& dir_path, const std::string& query);
void generateGrepOutput(State* state);
bool filePathContainsSubstring(const std::filesystem::path& filePath, const std::string& query);
bool shouldIgnoreFile(const std::filesystem::path& path);
std::vector<std::filesystem::path> findFiles(const std::filesystem::path& dir_path, const std::string& query);
void generateFindFileOutput(State* state);
unsigned int w(State* state);
unsigned int b(State* state);
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
void sanityCheckRowColOutOfBounds(State* state);
void sanityCheckDocumentEmpty(State* state);
int minimum(int a, int b);
int maximum(int a, int b);
