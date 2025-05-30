#include "fileExplorerNode.h"
#include "helper.h"

FileExplorerNode::FileExplorerNode()
{
}

void FileExplorerNode::close()
{
	if (this->isFolder) {
		this->isOpen = false;
		this->children.clear();
	}
}

int32_t FileExplorerNode::getChildIndex(FileExplorerNode *node)
{
	FileExplorerNode *current = this;
	int32_t output = 0;
	for (uint32_t j = 0; j < current->children.size(); j++) {
		auto s = current->children[j].path.string();
		if (s == safeSubstring(node->path.string(), 0, s.length())) {
			return output + 1;
		} else {
			output += current->children[j].getTotalChildren();
		}
	}
	return output;
}

int32_t FileExplorerNode::expand(std::string input)
{
	std::stringstream ss(input);
	std::string line;
	std::vector<std::string> path;
	while (getline(ss, line, '/')) {
		if (!line.empty()) {
			path.push_back(line);
		}
	}
	if (path.empty()) {
		return 0;
	}
	FileExplorerNode *current = this;
	bool found = false;
	int32_t output = 0;
	for (uint32_t i = 0; i < path.size(); i++) {
		if (!current->isOpen) {
			current->open();
		}
		found = false;
		if (current->children.empty()) {
			return 0;
		}
		for (uint32_t j = 0; j < current->children.size(); j++) {
			if (current->children[j].name == path[i]) {
				current = &current->children[j];
				found = true;
				output++;
				break;
			} else {
				output += current->children[j].getTotalChildren();
			}
		}
		if (!found) {
			return 0;
		}
	}
	return output;
}

int32_t FileExplorerNode::getTotalChildren()
{
	int32_t total = 1;
	if (this->isOpen) {
		for (uint32_t i = 0; i < this->children.size(); i++) {
			total += this->children[i].getTotalChildren();
		}
	}
	return total;
}

void FileExplorerNode::refresh()
{
	if (this->isFolder) {
		this->isOpen = true;
		std::vector<FileExplorerNode> newChildren;
		bool found = false;
		for (const auto &entry : std::filesystem::directory_iterator(this->path)) {
			found = false;
			for (uint32_t i = 0; i < this->children.size(); i++) {
				if (this->children[i].path == entry.path()) {
					newChildren.push_back(this->children[i]);
					found = true;
					break;
				}
			}
			if (!found) {
				newChildren.push_back(FileExplorerNode(entry.path(), this));
			}
		}
		this->children = newChildren;
		std::sort(this->children.begin(), this->children.end(),
			  [](const FileExplorerNode a, const FileExplorerNode b) {
				  if (a.isFolder && !b.isFolder) {
					  return true;
				  } else if (!a.isFolder && b.isFolder) {
					  return false;
				  }
				  return a.name < b.name;
			  });
	}
}

void FileExplorerNode::remove()
{
	if (this->parent != nullptr) {
		std::filesystem::remove_all(this->path);
	}
}

void FileExplorerNode::open()
{
	if (this->isFolder && !this->isOpen) {
		this->isOpen = true;
		for (const auto &entry : std::filesystem::directory_iterator(this->path)) {
			this->children.push_back(FileExplorerNode(entry.path(), this));
		}
		std::sort(this->children.begin(), this->children.end(),
			  [](const FileExplorerNode a, const FileExplorerNode b) {
				  if (a.isFolder && !b.isFolder) {
					  return true;
				  } else if (!a.isFolder && b.isFolder) {
					  return false;
				  }
				  return a.name < b.name;
			  });
	}
}

FileExplorerNode *FileExplorerNode::getNode(int32_t n)
{
	if (n < 0) {
		return nullptr;
	}
	std::stack<FileExplorerNode *> stack;
	stack.push(this);
	int32_t count = 0;
	while (!stack.empty()) {
		FileExplorerNode *current = stack.top();
		stack.pop();
		if (count == n) {
			return current;
		}
		count++;
		for (auto it = current->children.rbegin(); it != current->children.rend(); ++it) {
			stack.push(&(*it));
		}
	}
	return nullptr;
}

FileExplorerNode::FileExplorerNode(const std::filesystem::path &path)
{
	this->path = path;
	this->name = path.filename();
	this->isFolder = std::filesystem::is_directory(path);
	this->isOpen = false;
	this->children = std::vector<FileExplorerNode>();
}

FileExplorerNode::FileExplorerNode(const std::filesystem::path &path, FileExplorerNode *parent)
{
	this->path = path;
	this->name = path.filename();
	this->isFolder = std::filesystem::is_directory(path);
	this->isOpen = false;
	this->children = std::vector<FileExplorerNode>();
	this->parent = parent;
}
