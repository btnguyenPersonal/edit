#include "util/testAutocomplete.h"
#include "util/testEasilyBreakableThings.h"
#include "util/testInsertLoggingCode.h"
#include "util/testMovement.h"
#include "util/testRead.h"
#include "util/testRender.h"
#include "util/testSanityChecks.h"
#include "util/testSave.h"
#include "util/testString.h"
#include "util/testTypingKeys.h"
#include "util/ratchet.h"

auto color_white = "\033[0m";
auto color_green = "\033[1;32m";
auto color_red = "\033[1;31m";

uint32_t pass = 0;
uint32_t fail = 0;
struct testSuiteRun failures = {"failures", {}};

void printSuiteRun(struct testSuiteRun suite) {
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

void printPassFail() {
	printf("\n");
	printf("    pass: %d\n", pass);
	printf("    fail: %d\n", fail);
	if (fail > 0) {
		printf("\n");
		printSuiteRun(failures);
	}
}

int main() {
	printSuiteRun(testSave());
	printSuiteRun(testRead());
	printSuiteRun(testSanityChecks());
	printSuiteRun(testTypingKeys());
	printSuiteRun(testAutocomplete());
	printSuiteRun(testString());
	printSuiteRun(testInsertLoggingCode());
	printSuiteRun(testMovement());
	printSuiteRun(testRender());
	printSuiteRun(testEasilyBreakableThings());
	printSuiteRun(ratchet());
	printPassFail();
}
