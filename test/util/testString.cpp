#include "testString.h"

struct testSuiteRun testString()
{
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		output.push_back({ "isNumber case 0", compare(isNumber("ffff"), false) });
		output.push_back({ "isNumber case 1", compare(isNumber("0102f983"), false) });
		output.push_back({ "isNumber case 2", compare(isNumber("0x000000"), false) });
		output.push_back({ "isNumber case 3", compare(isNumber("82312973a"), false) });
		output.push_back({ "isNumber case 4", compare(isNumber("a1239123"), false) });
		output.push_back({ "isNumber case 5", compare(isNumber("a99999999"), false) });
		output.push_back({ "isNumber case 6", compare(isNumber("0000000O0"), false) });
		output.push_back({ "isNumber case 7", compare(isNumber("-0"), false) });
		output.push_back({ "isNumber case 8", compare(isNumber("-1"), false) });
	}

	{
		output.push_back({ "isNumber case 0", compare(isNumber("0"), true) });
		output.push_back({ "isNumber case 1", compare(isNumber("01"), true) });
		output.push_back({ "isNumber case 2", compare(isNumber("999990"), true) });
		output.push_back({ "isNumber case 3", compare(isNumber("999999990"), true) });
		output.push_back({ "isNumber case 4", compare(isNumber("377310"), true) });
		output.push_back({ "isNumber case 5", compare(isNumber("9"), true) });
	}

	{
		output.push_back({ "buildNumberFromString case 0", compare(buildNumberFromString("fff"), 0) });
		output.push_back({ "buildNumberFromString case 1", compare(buildNumberFromString("-1"), 0) });
		output.push_back({ "buildNumberFromString case 2", compare(buildNumberFromString("-2"), 0) });
		output.push_back({ "buildNumberFromString case 3", compare(buildNumberFromString("-0"), 0) });
		output.push_back({ "buildNumberFromString case 4", compare(buildNumberFromString("0xa"), 0) });
		output.push_back({ "buildNumberFromString case 5", compare(buildNumberFromString("0"), 0) });
		output.push_back({ "buildNumberFromString case 6", compare(buildNumberFromString("1"), 1) });
		output.push_back({ "buildNumberFromString case 7", compare(buildNumberFromString("9"), 9) });
		output.push_back({ "buildNumberFromString case 8", compare(buildNumberFromString("19991"), 19991) });
		output.push_back({ "buildNumberFromString case 9", compare(buildNumberFromString("0041231"), 41231) });
	}

	{
		output.push_back({ "isAlphanumeric case 0", compare(isAlphanumeric('0'), true) });
		output.push_back({ "isAlphanumeric case 1", compare(isAlphanumeric('1'), true) });
		output.push_back({ "isAlphanumeric case 2", compare(isAlphanumeric('x'), true) });
		output.push_back({ "isAlphanumeric case 3", compare(isAlphanumeric('a'), true) });
		output.push_back({ "isAlphanumeric case 4", compare(isAlphanumeric('_'), true) });
		output.push_back({ "isAlphanumeric case 5", compare(isAlphanumeric('*'), false) });
		output.push_back({ "isAlphanumeric case 6", compare(isAlphanumeric('/'), false) });
		output.push_back({ "isAlphanumeric case 7", compare(isAlphanumeric('^'), false) });
		output.push_back({ "isAlphanumeric case 8", compare(isAlphanumeric('&'), false) });
		output.push_back({ "isAlphanumeric case 9", compare(isAlphanumeric('%'), false) });
		output.push_back({ "isAlphanumeric case 10", compare(isAlphanumeric('('), false) });
		output.push_back({ "isAlphanumeric case 11", compare(isAlphanumeric(')'), false) });
		output.push_back({ "isAlphanumeric case 12", compare(isAlphanumeric(' '), false) });
	}

	{
		output.push_back({ "isWhitespace case 0", compare(isWhitespace(' '), true) });
		output.push_back({ "isWhitespace case 1", compare(isWhitespace('\t'), true) });
		output.push_back({ "isWhitespace case 2", compare(isWhitespace('0'), false) });
		output.push_back({ "isWhitespace case 3", compare(isWhitespace('1'), false) });
		output.push_back({ "isWhitespace case 4", compare(isWhitespace('a'), false) });
		output.push_back({ "isWhitespace case 5", compare(isWhitespace('z'), false) });
		output.push_back({ "isWhitespace case 6", compare(isWhitespace('{'), false) });
		output.push_back({ "isWhitespace case 7", compare(isWhitespace(')'), false) });
	}

	{
		output.push_back({ "safeSubstring case 0", compare(safeSubstring("hello world", 0, 0), "") });
		output.push_back({ "safeSubstring case 1", compare(safeSubstring("hello world", 20, 0), "") });
		output.push_back({ "safeSubstring case 2", compare(safeSubstring("hello world", 20, 1), "") });
		output.push_back({ "safeSubstring case 3", compare(safeSubstring("hello world", -1, 1), "") });
		output.push_back({ "safeSubstring case 4", compare(safeSubstring("hello world", 0, 1), "h") });
		output.push_back({ "safeSubstring case 5", compare(safeSubstring("hello world", 1, 1), "e") });
		output.push_back({ "safeSubstring case 6", compare(safeSubstring("hello world", 2, 1), "l") });
		output.push_back({ "safeSubstring case 7", compare(safeSubstring("hello world", 3, 1), "l") });
		output.push_back({ "safeSubstring case 8", compare(safeSubstring("hello world", 4, 1), "o") });
		output.push_back({ "safeSubstring case 9", compare(safeSubstring("hello world", 5, 1), " ") });
		output.push_back({ "safeSubstring case 10", compare(safeSubstring("hello world", 6, 1), "w") });
		output.push_back({ "safeSubstring case 11", compare(safeSubstring("hello world", 7, 1), "o") });
		output.push_back({ "safeSubstring case 12", compare(safeSubstring("hello world", 8, 1), "r") });
		output.push_back({ "safeSubstring case 13", compare(safeSubstring("hello world", 9, 1), "l") });
		output.push_back({ "safeSubstring case 14", compare(safeSubstring("hello world", 10, 1), "d") });
		output.push_back({ "safeSubstring case 15", compare(safeSubstring("hello world", 10, 1), "d") });
		output.push_back({ "safeSubstring case 16", compare(safeSubstring("hello world", 0, 100), "hello world") });
		output.push_back({ "safeSubstring case 17", compare(safeSubstring("hello world", 0, 10), "hello worl") });
		output.push_back({ "safeSubstring case 18", compare(safeSubstring("hello world", 0, 11), "hello world") });
		output.push_back({ "safeSubstring case 19", compare(safeSubstring("hello world", 6, 4), "worl") });
		output.push_back({ "safeSubstring case 20", compare(safeSubstring("hello world", 6, 4), "worl") });
		output.push_back({ "safeSubstring case 21", compare(safeSubstring("hello world", -1), "") });
		output.push_back({ "safeSubstring case 22", compare(safeSubstring("hello world", 0), "hello world") });
		output.push_back({ "safeSubstring case 23", compare(safeSubstring("hello world", 1), "ello world") });
		output.push_back({ "safeSubstring case 24", compare(safeSubstring("hello world", 2), "llo world") });
		output.push_back({ "safeSubstring case 25", compare(safeSubstring("hello world", 3), "lo world") });
		output.push_back({ "safeSubstring case 26", compare(safeSubstring("hello world", 4), "o world") });
		output.push_back({ "safeSubstring case 27", compare(safeSubstring("hello world", 5), " world") });
		output.push_back({ "safeSubstring case 28", compare(safeSubstring("hello world", 6), "world") });
		output.push_back({ "safeSubstring case 29", compare(safeSubstring("hello world", 7), "orld") });
		output.push_back({ "safeSubstring case 30", compare(safeSubstring("hello world", 8), "rld") });
		output.push_back({ "safeSubstring case 31", compare(safeSubstring("hello world", 9), "ld") });
		output.push_back({ "safeSubstring case 32", compare(safeSubstring("hello world", 10), "d") });
		output.push_back({ "safeSubstring case 33", compare(safeSubstring("hello world", 11), "") });
	}

	{
		output.push_back({ "replace case 0", compare(replace("aaaaaaaaaa", "a", "b"), "bbbbbbbbbb") });
		output.push_back({ "replace case 1", compare(replace("aaaaaaaaaa", "aa", "b"), "bbbbb") });
		output.push_back({ "replace case 2", compare(replace("for (int i = 0; i < a.size(); i++) {", "i", "j"), "for (jnt j = 0; j < a.sjze(); j++) {") });
		output.push_back({ "replace case 3", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "temp", "j"), "for (int j = 0; j < a.size(); j++) {") });
		output.push_back({ "replace case 4", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "+", "-"), "for (int temp = 0; temp < a.size(); temp--) {") });
		output.push_back({ "replace case 5", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "", " "), "for (int temp = 0; temp < a.size(); temp++) {") });
		output.push_back({ "replace case 6", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", " ", ""), "for(inttemp=0;temp<a.size();temp++){") });
		output.push_back({ "replace case 7", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "z", "zzzzz"), "for (int temp = 0; temp < a.sizzzzze(); temp++) {") });
		output.push_back({ "replace case 8", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "z", "z"), "for (int temp = 0; temp < a.size(); temp++) {") });
		output.push_back({ "replace case 9", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "temp", "temptemp"), "for (int temptemp = 0; temptemp < a.size(); temptemp++) {") });
		output.push_back({ "replace case 10", compare(replace("for (int temp = 0; temp < a.size(); temp++) {", "temp", "temptemp"), "for (int temptemp = 0; temptemp < a.size(); temptemp++) {") });
	}

	{
		output.push_back({ "splitByChar case 1", compare(splitByChar("%s/from/to/g", '/'), { "%s", "from", "to", "g" }) });
		output.push_back({ "splitByChar case 2", compare(splitByChar("gs/from/to/g", '/'), { "gs", "from", "to", "g" }) });
		output.push_back({ "splitByChar case 3", compare(splitByChar("bingbong/bing", '/'), { "bingbong", "bing" }) });
		output.push_back({ "splitByChar case 4", compare(splitByChar(" bing bong bing ", ' '), { "", "bing", "bong", "bing", "" }) });
		output.push_back({ "splitByChar case 5", compare(splitByChar("   ", ' '), { "", "", "", "" }) });
		output.push_back({ "splitByChar case 6", compare(splitByChar("b b", 'b'), { "", " ", "" }) });
		output.push_back({ "splitByChar case 7", compare(splitByChar("/from/to/", '/'), { "", "from", "to", "" }) });
		output.push_back({ "splitByChar case 8", compare(splitByChar("from", '/'), { "from" }) });
	}

	{
		output.push_back({ "getIndexFirstNonSpace case 0", compareInt(getIndexFirstNonSpace("%s/from/to/g", ' '), 0) });
		output.push_back({ "getIndexFirstNonSpace case 1", compareInt(getIndexFirstNonSpace(" %s/from/to/g", ' '), 1) });
		output.push_back({ "getIndexFirstNonSpace case 2", compareInt(getIndexFirstNonSpace(" 	%s/from/to/g", ' '), 1) });
		output.push_back({ "getIndexFirstNonSpace case 3", compareInt(getIndexFirstNonSpace("	 	%s/from/to/g", ' '), 0) });
		output.push_back({ "getIndexFirstNonSpace case 4", compareInt(getIndexFirstNonSpace("	 	%s/from/to/g", '	'), 1) });
		output.push_back({ "getIndexFirstNonSpace case 5", compareInt(getIndexFirstNonSpace("        %s/from/to/g", ' '), 8) });
		output.push_back({ "getIndexFirstNonSpace case 6", compareInt(getIndexFirstNonSpace("			%s/from/to/g", '	'), 3) });
	}

	return { "test/util/testString.cpp", output };
}
