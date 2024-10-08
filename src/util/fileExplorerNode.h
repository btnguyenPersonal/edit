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
    std::vector<FileExplorerNode> children;
    FileExplorerNode();
    FileExplorerNode(const std::filesystem::path& path);
    static FileExplorerNode* getFileExplorerNode(FileExplorerNode* node, int index);
    int getTotalChildren();
    int expand(std::string input);
    void open();
    void close();
};
