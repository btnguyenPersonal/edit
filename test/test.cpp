#include "./util/testSave.cpp"
#include "./util/testRead.cpp"
#include "./util/testSanityChecks.cpp"

auto color_white = "\033[0m";
auto color_green = "\033[1;32m";
auto color_red = "\033[1;31m";

void printSuiteRun(struct testSuiteRun suite) {
	printf("    %s\n", suite.file.c_str());
	for (uint32_t i = 0; i < suite.runs.size(); i++) {
		if (suite.runs[i].result.equal) {
			printf("        %so%s %s\n", color_green, color_white, suite.runs[i].name.c_str());
		} else {
			printf("        %sx%s %s\n", color_red, color_white, suite.runs[i].name.c_str());
			printf("%s\n", suite.runs[i].result.error.c_str());
		}
	}
}

int main() {
	 printSuiteRun(testSave());
	 printSuiteRun(testRead());
	 printSuiteRun(testSanityChecks());
}
