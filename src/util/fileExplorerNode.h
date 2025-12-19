#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <stack>

class FileExplorerNode {
    public:
	std::filesystem::path path;
	std::string name;
	bool isFolder;
	bool isOpen;
	std::vector<FileExplorerNode *> children;
	FileExplorerNode *parent;
	FileExplorerNode();
	~FileExplorerNode();
	FileExplorerNode(const std::filesystem::path &path);
	FileExplorerNode(const std::filesystem::path &path, FileExplorerNode *parent);
	FileExplorerNode *getNode(int32_t n);
	void sortChildren();
	int32_t getTotalChildren();
	int32_t expand(std::string input);
	int32_t getChildIndex(FileExplorerNode *node);
	void remove();
	void open();
	void refresh();
	void close();
};
