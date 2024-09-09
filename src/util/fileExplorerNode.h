#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

class FileExplorerNode {
public:
    std::filesystem::path path;
    std::string name;
    bool isFolder;
    bool isOpen;
    std::vector<FileExplorerNode> children;
    FileExplorerNode();
    FileExplorerNode(const std::filesystem::path& path);
    void open();
    void close();
};
