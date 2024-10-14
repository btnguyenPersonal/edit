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

void sendFileExplorerKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (c == ctrl('g')) {
        state->mode = GREP;
    } else if (c == ctrl('p')) {
        state->mode = FINDFILE;
    } else if (c == ':') {
        state->mode = COMMANDLINE;
    } else if (c == '-') {
        state->fileExplorerWindowLine = 0;
        state->fileExplorerIndex = 0;
        auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
        node->close();
        node->open();
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
            }
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
    } else if (c == ctrl('t')) {
        state->mode = SHORTCUTS;
        state->fileExplorerOpen = false;
    } else if (c == ctrl('u')) {
        for (unsigned int i = 0; i < state->maxY / 2; i++) {
            if (state->fileExplorerIndex > 0) {
                state->fileExplorerIndex--;
            }
        }
    } else if (c == 'k') {
        if (state->fileExplorerIndex > 0) {
            state->fileExplorerIndex--;
        }
    } else if (c == ctrl('d')) {
        for (unsigned int i = 0; i < state->maxY / 2; i++) {
            if (state->fileExplorerIndex + 1 < state->fileExplorer->getTotalChildren()) {
                state->fileExplorerIndex++;
            }
        }
    } else if (c == 'j') {
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
            state->changeFile(relativePath);
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
            state->changeFile(relativePath);
        }
    } else {
        return;
    }
    // TODO add bounds checking
    centerFileExplorer(state);
}
