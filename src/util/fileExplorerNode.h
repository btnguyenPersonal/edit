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
    FileExplorerNode* parent;
    FileExplorerNode();
    FileExplorerNode(const std::filesystem::path& path);
    FileExplorerNode(const std::filesystem::path& path, FileExplorerNode* parent);
    static FileExplorerNode* getFileExplorerNode(FileExplorerNode* node, int index);
    int getTotalChildren();
    int expand(std::string input);
    void remove();
    void open();
    void refresh();
    void close();
};
