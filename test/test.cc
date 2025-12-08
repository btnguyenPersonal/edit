#include "./util/testSave.h"
#include "./util/testRead.h"
#include "./util/testSanityChecks.h"
#include "./util/testTypingKeys.h"
#include "./util/testAutocomplete.h"

auto color_white = "\033[0m";
auto color_green = "\033[1;32m";
auto color_red = "\033[1;31m";

uint32_t pass = 0;
uint32_t fail = 0;
struct testSuiteRun failures = {"failures", {}};

void printSuiteRun(struct testSuiteRun suite)
{
	printf("    %s\n", suite.file.c_str());
	for (uint32_t i = 0; i < suite.runs.size(); i++) {
		if (suite.runs[i].result.equal) {
			pass++;
			printf("        %so%s %s\n", color_green, color_white, suite.runs[i].name.c_str());
		} else {
			fail++;
			failures.runs.push_back(suite.runs[i]);
			printf("        %sx%s %s\n", color_red, color_white, suite.runs[i].name.c_str());
			printf("%s\n", suite.runs[i].result.error.c_str());
		}
	}
}

int main()
{
	printSuiteRun(testSave());
	printSuiteRun(testRead());
	printSuiteRun(testSanityChecks());
	printSuiteRun(testTypingKeys());
	printSuiteRun(testAutocomplete());
	printf("\n    pass: %d\n", pass);
	printf("    fail: %d\n", fail);
	if (fail > 0) {
		printf("\n");
		printSuiteRun(failures);
		return 1;
	}
}
