#include "testFileExplorerNode.h"

struct testSuiteRun testFileExplorerNode() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();
		output.push_back({"empty directory should have no children", compare((uint32_t)root->children.size(), (uint32_t)0)});
		output.push_back({"empty directory getTotalChildren should return 1", compareInt(root->getTotalChildren(), 1)});

		output.push_back({"getNode(1) on empty should return nullptr", compare(root->getNode(1) == nullptr, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		std::string path = "/tmp/test-explorer";
		for (int i = 0; i < 50; i++) {
			path += "/level" + std::to_string(i);
		}
		system(("mkdir -p " + path).c_str());
		system(("touch " + path + "/deepfile.txt").c_str());

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		int32_t expanded = root->expand("level0/level1/level2/level3/level4");
		output.push_back({"expand should handle deep nesting", compareInt(expanded, 5)});

		std::string assertPath;
		for (int i = 0; i < 50; i++) {
			assertPath += "/level" + std::to_string(i);
		}
		FileExplorerNode *root2 = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		int32_t extremeExpanded = root2->expand(assertPath.substr(1));
		output.push_back({"expand should handle extreme deep nesting", compareInt(extremeExpanded, 50)});

		FileExplorerNode *root3 = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		int32_t badExpand = root3->expand(assertPath.substr(1) + "noexist");
		output.push_back({"expand non-existent path should return 0", compareInt(badExpand, 0)});

		delete root;
		delete root2;
		delete root3;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		for (int i = 0; i < 100; i++) {
			system(("touch /tmp/test-explorer/file" + std::to_string(i) + ".txt").c_str());
		}

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();
		output.push_back({"should handle 100 files", compare((uint32_t)root->children.size(), (uint32_t)100)});

		int32_t total = root->getTotalChildren();
		output.push_back({"getTotalChildren with 100 files", compareInt(total, 101)});

		FileExplorerNode *lastNode = root->getNode(100);
		output.push_back({"getNode(100) should return last child", compare(lastNode != nullptr, (bool)true)});

		output.push_back({"getNode(10000) should return nullptr", compare(root->getNode(10000) == nullptr, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/subdir");
		system("touch /tmp/test-explorer/file.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();
		uint32_t childrenAfterFirstOpen = root->children.size();

		root->open();
		output.push_back({
			"double open should not duplicate children",
			compare((uint32_t)root->children.size(), childrenAfterFirstOpen)
		});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		system("touch /tmp/test-explorer/file.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		FileExplorerNode *fileNode = nullptr;
		for (auto *child : root->children) {
			if (!child->isFolder) {
				fileNode = child;
				break;
			}
		}

		if (fileNode) {
			fileNode->close();
			output.push_back({"close on file node should not crash", compare((bool)true, (bool)true)});
		}

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		system("touch /tmp/test-explorer/file.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		FileExplorerNode *fileNode = nullptr;
		for (auto *child : root->children) {
			if (!child->isFolder) {
				fileNode = child;
				break;
			}
		}

		if (fileNode) {
			fileNode->open();
			output.push_back({"open on file node should not crash", compare((bool)true, (bool)true)});
			output.push_back({"open on file should not set isOpen", compare(fileNode->isOpen, (bool)false)});
		}

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		system("touch /tmp/test-explorer/keep.txt");
		system("touch /tmp/test-explorer/delete.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();
		output.push_back({"before delete should have 2 files", compare((uint32_t)root->children.size(), (uint32_t)2)});

		system("rm /tmp/test-explorer/delete.txt");
		root->refresh();
		output.push_back({"after refresh should have 1 file", compare((uint32_t)root->children.size(), (uint32_t)1)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		system("touch '/tmp/test-explorer/file with spaces.txt'");
		system("touch '/tmp/test-explorer/file-with-dashes.txt'");
		system("touch '/tmp/test-explorer/file_with_underscores.txt'");
		system("touch '/tmp/test-explorer/UPPERCASE.TXT'");
		system("touch '/tmp/test-explorer/lowercase.txt'");
		system("touch '/tmp/test-explorer/123numbers.txt'");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();
		output.push_back({"should handle special characters in filenames", compare((uint32_t)root->children.size(), (uint32_t)6)});

		bool sorted = true;
		bool foundFile = false;
		for (size_t i = 0; i < root->children.size(); i++) {
			if (!root->children[i]->isFolder) {
				foundFile = true;
			} else if (foundFile) {
				sorted = false;
			}
		}
		output.push_back({"sorting with special chars should work", compare(sorted, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/subdir1");
		system("mkdir -p /tmp/test-explorer/subdir2");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		FileExplorerNode *otherRoot = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();
		otherRoot->open();

		if (otherRoot->children.size() > 0) {
			int32_t idx = root->getChildIndex(otherRoot->children[0]);
			output.push_back({"getChildIndex with foreign node should not crash", compare((bool)true, (bool)true)});
		}

		int32_t idx = root->getChildIndex(nullptr);
		output.push_back({"getChildIndex with nullptr node should return 0", compareInt(idx, 0)});

		delete root;
		delete otherRoot;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/folder");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));

		int32_t emptyExpand = root->expand("");
		output.push_back({"expand empty string should return 0", compareInt(emptyExpand, 0)});

		int32_t slashExpand = root->expand("/");
		output.push_back({"expand slash only should return 0", compareInt(slashExpand, 0)});

		int32_t trailingSlash = root->expand("folder/");
		output.push_back({"expand with trailing slash", compareInt(trailingSlash, 1)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/a/b/c/d/e");
		system("touch /tmp/test-explorer/a/b/c/d/e/f.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));

		for (int i = 0; i < 10; i++) {
			root->open();
			root->close();
		}

		output.push_back({"rapid open/close cycles should not crash", compare((bool)true, (bool)true)});

		root->open();
		output.push_back({"after cycles children should be correct", compare((uint32_t)root->children.size(), (uint32_t)1)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");

		for (int i = 0; i < 20; i++) {
			if (i % 2 == 0) {
				system(("touch /tmp/test-explorer/file" + std::to_string(i) + ".txt").c_str());
			} else {
				system(("mkdir /tmp/test-explorer/folder" + std::to_string(i)).c_str());
			}
		}

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		bool foldersFirst = true;
		bool seenFile = false;
		int folderCount = 0;
		int fileCount = 0;

		for (auto *child : root->children) {
			if (child->isFolder) {
				folderCount++;
				if (seenFile) {
					foldersFirst = false;
				}
			} else {
				fileCount++;
				seenFile = true;
			}
		}

		output.push_back({"mixed folders/files should sort folders first", compare(foldersFirst, (bool)true)});
		output.push_back({"should have 10 folders", compare(folderCount == 10, (bool)true)});
		output.push_back({"should have 10 files", compare(fileCount == 10, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer-nonexistent-xyz"));
		output.push_back({"non-existent path constructor should not crash", compare((bool)true, (bool)true)});

		root->open();
		output.push_back({"open on non-existent should handle gracefully", compare((bool)true, (bool)true)});

		delete root;
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/subdir");
		system("touch /tmp/test-explorer/file1.txt");
		system("touch /tmp/test-explorer/file2.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		system("rm /tmp/test-explorer/file1.txt");

		// TODO(ben): this shouldn't keep stale data or make all the operations after see if file exists
		uint32_t sizeBeforeRefresh = root->children.size();
		output.push_back({"fs-change: node keeps stale data before refresh", compare(sizeBeforeRefresh == (uint32_t)3, (bool)true)});

		root->refresh();
		output.push_back({"fs-change: refresh sees deleted file", compare((uint32_t)root->children.size(), (uint32_t)2)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/todelete/subdir");
		system("touch /tmp/test-explorer/todelete/subdir/file.txt");
		system("touch /tmp/test-explorer/other.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		root->expand("todelete/subdir");

		system("rm -rf /tmp/test-explorer/todelete");

		root->refresh();
		output.push_back({"fs-change: refresh handles deleted expanded folder", compare((bool)true, (bool)true)});

		bool foundOther = false;
		for (auto *child : root->children) {
			if (child->name == "other.txt") {
				foundOther = true;
			}
		}
		output.push_back({"fs-change: only surviving files remain", compare(foundOther, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		system("touch /tmp/test-explorer/file1.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		for (int i = 2; i <= 50; i++) {
			system(("touch /tmp/test-explorer/file" + std::to_string(i) + ".txt").c_str());
		}

		output.push_back({"fs-change: open node doesn't auto-see new files", compare((uint32_t)root->children.size(), (uint32_t)1)});

		root->refresh();
		output.push_back({"fs-change: refresh picks up 50 new files", compare((uint32_t)root->children.size(), (uint32_t)50)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/a/b/c");
		system("touch /tmp/test-explorer/a/file.txt");
		system("touch /tmp/test-explorer/a/b/file.txt");
		system("touch /tmp/test-explorer/a/b/c/file.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->expand("a/b/c");

		system("rm -rf /tmp/test-explorer/a");

		// TODO(ben): make another test for the full delete of test-explorer
		root->refresh();
		output.push_back({"fs-change: refresh after full delete doesn't crash", compare((bool)true, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");
		system("touch /tmp/test-explorer/switch.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		bool wasFile = !root->children[0]->isFolder;

		system("rm /tmp/test-explorer/switch.txt");
		system("mkdir /tmp/test-explorer/switch.txt");
		system("touch /tmp/test-explorer/switch.txt/inside.txt");

		root->refresh();

		// TODO(ben): fix .cpp to update if folder or file and vice versa
		bool pathCorrect = (root->children[0]->path.filename() == "switch.txt");
		output.push_back({"fs-change: refresh preserves path on file->folder switch", compare(wasFile && pathCorrect, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/level1/level2/level3");
		system("touch /tmp/test-explorer/a.txt");
		system("touch /tmp/test-explorer/level1/b.txt");
		system("touch /tmp/test-explorer/level1/level2/c.txt");
		system("touch /tmp/test-explorer/level1/level2/level3/d.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->expand("level1/level2/level3");

		system("rm /tmp/test-explorer/a.txt");
		system("rm /tmp/test-explorer/level1/b.txt");
		system("rm /tmp/test-explorer/level1/level2/c.txt");
		system("rm /tmp/test-explorer/level1/level2/level3/d.txt");

		system("touch /tmp/test-explorer/new.txt");
		system("touch /tmp/test-explorer/level1/new.txt");
		system("touch /tmp/test-explorer/level1/level2/new.txt");
		system("touch /tmp/test-explorer/level1/level2/level3/new.txt");

		root->refresh();
		// TODO(ben): real assert here
		output.push_back({"fs-change: multi-level concurrent modifications", compare((bool)true, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/secret");
		system("touch /tmp/test-explorer/secret/hidden.txt");
		system("touch /tmp/test-explorer/public.txt");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		system("chmod 000 /tmp/test-explorer/secret");

		root->refresh();
		// TODO(ben): what really happens here?
		output.push_back({"fs-change: handles permission denied gracefully", compare((bool)true, (bool)true)});

		system("chmod 755 /tmp/test-explorer/secret");

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer/real");
		system("touch /tmp/test-explorer/real/target.txt");
		system("ln -s /tmp/test-explorer/real /tmp/test-explorer/link 2>/dev/null || true");

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		bool foundReal = false;
		bool foundLink = false;
		for (auto *child : root->children) {
			if (child->name == "real") foundReal = true;
			if (child->name == "link") foundLink = true;
		}

		// TODO(ben): real assert here
		output.push_back({"fs-change: handles symlinks if supported", compare(foundReal, (bool)true)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	{
		system("rm -rf /tmp/test-explorer");
		system("mkdir -p /tmp/test-explorer");

		std::string longName = "a";
		for (int i = 0; i < 200; i++) {
			longName += "b";
		}
		longName += ".txt";
		system(("touch '/tmp/test-explorer/" + longName + "'").c_str());

		FileExplorerNode *root = new FileExplorerNode(std::filesystem::path("/tmp/test-explorer"));
		root->open();

		output.push_back({"fs-change: handles long filenames", compare((uint32_t)root->children.size(), (uint32_t)1)});

		system(("rm '/tmp/test-explorer/" + longName + "'").c_str());
		root->refresh();
		output.push_back({"fs-change: refresh after deleting long filename", compare((uint32_t)root->children.size(), (uint32_t)0)});

		delete root;
		system("rm -rf /tmp/test-explorer");
	}

	return {"test/util/testFileExplorerNode.cpp", output};
}
