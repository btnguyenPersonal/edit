#include "./ignore.h"

#include <vector>

bool isTestFile(const std::string &filepath)
{
	return filepath.find(".spec") != std::string::npos || filepath.find(".test") != std::string::npos ||
	       filepath.find(".cy") != std::string::npos;
}

bool shouldIgnoreFile(const std::filesystem::path &path)
{
	std::vector<std::string> allowList = { "[...nextauth]", ".github", ".gitconfig", ".gitignore" };
	for (uint32_t i = 0; i < allowList.size(); i++) {
		if (path.string().find(allowList[i]) != std::string::npos) {
			return false;
		}
	}
	std::vector<std::string> ignoreList = {
		".nx/",	    ".mozilla/",    ".vim/",	 "nyc_output/",	  "results/",
		"target/",  "resources/",   ".git",	 "node_modules/", "build/",
		"dist/",    "cdk.out/",	    ".next/",	 "tmp/",	  "__pycache__/",
		"autogen/", "coverage/",    "assets/",	 "extra/",	  ".jar",
		".jpg",	    ".jpeg",	    ".png",	 ".pdf",	  "package-lock.json",
		".cache/",  ".eslintcache", ".DS_Store", "snapshots/"
	};
	for (uint32_t i = 0; i < ignoreList.size(); i++) {
		if (path.string().find(ignoreList[i]) != std::string::npos) {
			return true;
		}
	}
	return false;
}

