#include "sendFileExplorerKeys.h"
#include "../util/fileExplorerNode.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/name.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendFileExplorerKeys(State* state, int32_t c) {
    try {
        if (c == 27) { // ESC
            state->mode = SHORTCUTS;
            state->fileExplorerOpen = false;
        } else if (c == ctrl('t')) {
            state->mode = SHORTCUTS;
        } else if (c == '-') {
            state->fileExplorerWindowLine = 0;
            state->fileExplorerIndex = 0;
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            node->close();
            node->open();
        } else if (c == KEY_BACKSPACE || c == ctrl('h')) {
            if (state->fileExplorerSize - 5 > 10) {
                state->fileExplorerSize -= 5;
            }
        } else if (c == ctrl('l')) {
            state->fileExplorerSize += 5;
        } else if (c == 'r') {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            node->refresh();
        } else if (c == 'R') {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            std::string name = inputName(state, node->name);
            if (name != "") {
                rename(state, node->path, name);
                node->parent->refresh();
                state->resetState((node->path.parent_path() / name).string());
                refocusFileExplorer(state, true);
            }
        } else if (c == 'X') {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            if (node->parent != nullptr) {
                try {
                    node->remove();
                    node->parent->refresh();
                } catch (const std::exception& e) {
                    state->status = "remove failed";
                }
            }
        } else if (c == ctrl('g')) {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            if (node->isFolder) {
                state->fileExplorerOpen = false;
                state->grepPath = node->path;
                state->mode = GREP;
                generateGrepOutput(state, true);
            }
        } else if (c == 'N') {
            std::string name = inputName(state, "");
            if (name != "") {
                auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
                if (node->isFolder) {
                    createFolder(state, node->path.string(), name);
                    node->refresh();
                } else {
                    createFolder(state, node->path.parent_path().string(), name);
                    node->parent->refresh();
                }
            }
        } else if (c == 'n') {
            std::string name = inputName(state, "");
            if (name != "") {
                auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
                if (node->isFolder) {
                    createFile(state, node->path.string(), name);
                    node->refresh();
                } else {
                    createFile(state, node->path.parent_path().string(), name);
                    node->parent->refresh();
                    state->resetState((node->path.parent_path() / name).string());
                }
                refocusFileExplorer(state, true);
            }
        } else if (c == 'p' || c == 'P') {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            if (node->isFolder) {
                pasteFileFromClipboard(state, node->path.string());
                node->refresh();
            } else {
                pasteFileFromClipboard(state, node->path.parent_path().string());
                node->parent->refresh();
            }
        } else if (c == 'y') {
            // TODO add copy for folders
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            copyFileToClipboard(state, node->path.string());
        } else if (c == ctrl('u')) {
            for (uint32_t i = 0; i < state->maxY / 2; i++) {
                if (state->fileExplorerIndex > 0) {
                    state->fileExplorerIndex--;
                }
            }
        } else if (c == 'k' || c == ctrl('p')) {
            if (state->fileExplorerIndex > 0) {
                state->fileExplorerIndex--;
            }
        } else if (c == ctrl('d')) {
            auto children = state->fileExplorer->getTotalChildren();
            for (uint32_t i = 0; i < state->maxY / 2; i++) {
                if (state->fileExplorerIndex + 1 < children) {
                    state->fileExplorerIndex++;
                }
            }
        } else if (c == 'j' || c == ctrl('n')) {
            if (state->fileExplorerIndex + 1 < state->fileExplorer->getTotalChildren()) {
                state->fileExplorerIndex++;
            }
        } else if (c == 'f') {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            if (node->isFolder) {
                if (node->isOpen) {
                    node->close();
                } else {
                    node->open();
                }
            } else {
                auto baseDir = std::filesystem::current_path();
                auto relativePath = std::filesystem::relative(node->path.string(), baseDir);
                state->resetState(relativePath);
            }
            state->mode = FILEEXPLORER;
        } else if (c == '\n') {
            auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
            if (node->isFolder) {
                if (node->isOpen) {
                    node->close();
                } else {
                    node->open();
                }
            } else {
                auto baseDir = std::filesystem::current_path();
                auto relativePath = std::filesystem::relative(node->path.string(), baseDir);
                state->resetState(relativePath);
                state->fileExplorerOpen = false;
            }
        } else {
            return;
        }
        if (state->fileExplorerIndex >= state->fileExplorer->getTotalChildren()) {
            state->fileExplorerIndex = state->fileExplorer->getTotalChildren() - 1;
        }
        centerFileExplorer(state);
    } catch (const std::exception& e) {
        state->status = "something went wrong with fileExplorer" + std::string(e.what());
    }
}
