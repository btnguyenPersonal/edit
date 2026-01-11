#include "sendFileExplorerKeys.h"
#include "../util/fileExplorerNode.h"
#include "../util/clipboard.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/prompt.h"
#include "../util/grep.h"
#include "../util/fileops.h"
#include "../util/display.h"
#include "../util/ctrl.h"
#include "../util/defines.h"
#include "../util/switchMode.h"
#include <ncurses.h>
#include <string>
#include <vector>

void pressEnterFileExplorer(State *state)
{
	auto node = state->explorer.root->getNode(state->explorer.index);
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
		state->explorer.open = false;
	}
}

void sendFileExplorerKeys(State *state, int32_t c)
{
	try {
		if (c == KEY_MOUSE) {
			MEVENT event;
			if (getmouse(&event) == OK) {
				if (event.bstate & BUTTON1_PRESSED) {
					if (event.y >= STATUS_BAR_LENGTH) {
						int32_t mouseIndex = state->explorer.windowLine + (event.y - STATUS_BAR_LENGTH);
						if (state->explorer.index == mouseIndex) {
							pressEnterFileExplorer(state);
						} else {
							if (mouseIndex < state->explorer.root->getTotalChildren()) {
								state->explorer.index = mouseIndex;
							} else {
								state->explorer.index = state->explorer.root->getTotalChildren();
								if (state->explorer.index > 0) {
									state->explorer.index--;
								}
							}
							auto node = state->explorer.root->getNode(state->explorer.index);
							if (node->isFolder) {
								pressEnterFileExplorer(state);
							}
						}
					}
				} else if (event.bstate & BUTTON4_PRESSED) {
					if (state->explorer.windowLine > 0) {
						state->explorer.windowLine--;
					}
#ifdef __APPLE__
				} else if (event.bstate & 0x8000000) {
#else
				} else if (event.bstate & BUTTON5_PRESSED) {
#endif
					if (state->explorer.windowLine + 1 < state->explorer.root->getTotalChildren()) {
						state->explorer.windowLine++;
					}
				}
				return;
			}
		} else if (c == 27) { // ESC
			switchMode(state, NORMAL);
			state->explorer.open = false;
		} else if (c == ctrl('t')) {
			switchMode(state, NORMAL);
		} else if (c == '-') {
			state->explorer.windowLine = 0;
			state->explorer.index = 0;
			auto node = state->explorer.root->getNode(state->explorer.index);
			node->close();
			node->open();
		} else if (c == KEY_BACKSPACE || c == ctrl('h')) {
			if (state->explorer.size - 5 > 10) {
				state->explorer.size -= 5;
			}
		} else if (c == ctrl('l')) {
			state->explorer.size += 5;
		} else if (c == 'r') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			node->refresh();
		} else if (c == 'R') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			state->prompt = "renaming " + node->name + ":";
			std::string name = prompt(state, node->name);
			if (name != "") {
				rename(state, node->path, name);
				node->parent->refresh();
			}
		} else if (c == ':') {
			switchMode(state, COMMAND);
		} else if (c == 'X') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			if (node->parent != nullptr) {
				try {
					node->remove();
					node->parent->refresh();
				} catch (const std::exception &e) {
					state->status = "remove failed";
				}
			}
		} else if (c == ctrl('g')) {
			auto node = state->explorer.root->getNode(state->explorer.index);
			if (node->isFolder) {
				state->explorer.open = false;
				state->grepPath = node->path;
				switchMode(state, GREP);
				state->showAllGrep = false;
				generateGrepOutput(state, true);
			} else {
				auto node = state->explorer.root->getNode(state->explorer.index);
				state->explorer.index = state->explorer.root->getChildIndex(node);
			}
		} else if (c == 'N') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			state->prompt = "new folder:";
			std::string name = prompt(state, "");
			if (name != "") {
				if (node->isFolder) {
					createFolder(node->path.string(), name);
					node->refresh();
				} else {
					createFolder(node->path.parent_path().string(), name);
					node->parent->refresh();
				}
			}
		} else if (c == 'n') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			state->prompt = "new file:";
			std::string name = prompt(state, "");
			if (name != "") {
				if (node->isFolder) {
					createFile(state, node->path.string(), name);
					node->refresh();
				} else {
					createFile(state, node->path.parent_path().string(), name);
					node->parent->refresh();
				}
				refocusFileExplorer(state, true);
			}
		} else if (c == 'p' || c == 'P') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			if (node->isFolder) {
				pasteFileFromClipboard(state, node->path.string());
				node->refresh();
			} else {
				pasteFileFromClipboard(state, node->path.parent_path().string());
				node->parent->refresh();
			}
		} else if (c == 'y') {
			auto node = state->explorer.root->getNode(state->explorer.index);
			copyPathToClipboard(state, node->path.string());
		} else if (c == ctrl('u')) {
			for (uint32_t i = 0; i < state->maxY / 2; i++) {
				if (state->explorer.index > 0) {
					state->explorer.index--;
				}
			}
		} else if (c == 'k' || c == ctrl('p')) {
			if (state->explorer.index > 0) {
				state->explorer.index--;
			}
		} else if (c == ctrl('d')) {
			auto children = state->explorer.root->getTotalChildren();
			for (uint32_t i = 0; i < state->maxY / 2; i++) {
				if (state->explorer.index + 1 < children) {
					state->explorer.index++;
				}
			}
		} else if (c == 'j' || c == ctrl('n')) {
			if (state->explorer.index + 1 < state->explorer.root->getTotalChildren()) {
				state->explorer.index++;
			}
		} else if (c == ctrl('r')) {
			auto node = state->explorer.root->getNode(state->explorer.index);
			auto relativePath = getRelativeToCurrent(state, node->path.string());
			state->status = relativePath;
			copyToClipboard(state, relativePath, false);
			state->explorer.open = false;
			switchMode(state, NORMAL);
		} else if (c == 'f') {
			auto node = state->explorer.root->getNode(state->explorer.index);
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
			switchMode(state, FILEEXPLORER);
		} else if (c == '\n') {
			pressEnterFileExplorer(state);
		} else {
			return;
		}
		putCursorBackOnScreenFileExplorer(state);
		if (state->explorer.windowLine < 0) {
			state->explorer.windowLine = 0;
		}
		if (state->explorer.index >= state->explorer.root->getTotalChildren()) {
			state->explorer.index = state->explorer.root->getTotalChildren() - 1;
		}
		if (state->explorer.index < 0) {
			state->explorer.index = 0;
		}
	} catch (const std::exception &e) {
		state->status = "something went wrong with fileExplorer" + std::string(e.what());
	}
}
