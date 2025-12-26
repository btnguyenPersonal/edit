#pragma once

#include <string>
#include <vector>
#include <cstdint>

uint32_t buildNumberFromString(std::string s);
std::vector<std::string> splitByChar(std::string text, char c);
std::string replace(std::string str, const std::string &from, const std::string &to);
int32_t getIndexFirstNonSpace(std::string s, char indentCharacter);
bool isWhitespace(char c);
std::string safeSubstring(const std::string &str, std::size_t pos, std::size_t len);
std::string safeSubstring(const std::string &str, std::size_t pos);
bool isAlphanumeric(char c);
bool isNumber(std::string s);
