#include "fileExplorerNode.h"

FileExplorerNode::FileExplorerNode() {
}

void FileExplorerNode::close() {
    if (this->isFolder) {
        this->isOpen = false;
        this->children.clear();
    }
}

void FileExplorerNode::open() {
    if (this->isFolder) {
        this->isOpen = true;
        for (const auto & entry : std::filesystem::directory_iterator(this->path)) {
            this->children.push_back(FileExplorerNode(entry.path()));
        }
        std::sort(this->children.begin(), this->children.end(), [](const FileExplorerNode a, const FileExplorerNode b) {
            if (a.isFolder && !b.isFolder) {
                return true;
            } else if (!a.isFolder && b.isFolder) {
                return false;
            }
            return a.name < b.name;
        });
    }
}

FileExplorerNode::FileExplorerNode(const std::filesystem::path& path) {
    this->path = path;
    this->name = path.filename();
    this->isFolder = std::filesystem::is_directory(path);
    this->isOpen = false;
    this->children = std::vector<FileExplorerNode>();
}