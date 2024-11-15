CC = g++-13

NPROCS = $(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu)
MAKEFLAGS += --jobs=$(NPROCS)

CFLAGS = -Wall -O2
LDFLAGS = -lncurses -pthread
DEPFLAGS = -MMD -MP

SRC_DIR = src
KEYBINDS_DIR = $(SRC_DIR)/keybinds
UTIL_DIR = $(SRC_DIR)/util
BUILD_DIR = build

SOURCES := $(SRC_DIR)/edit.cpp \
          $(UTIL_DIR)/helper.cpp \
          $(UTIL_DIR)/render.cpp \
          $(UTIL_DIR)/fileExplorerNode.cpp \
          $(UTIL_DIR)/state.cpp \
          $(UTIL_DIR)/clipboard.cpp \
          $(UTIL_DIR)/history.cpp \
          $(UTIL_DIR)/indent.cpp \
          $(UTIL_DIR)/comment.cpp \
          $(UTIL_DIR)/insertLoggingCode.cpp \
          $(UTIL_DIR)/query.cpp \
          $(UTIL_DIR)/name.cpp \
          $(KEYBINDS_DIR)/sendKeys.cpp \
          $(KEYBINDS_DIR)/sendVisualKeys.cpp \
          $(KEYBINDS_DIR)/sendBlameKeys.cpp \
          $(KEYBINDS_DIR)/sendFindFileKeys.cpp \
          $(KEYBINDS_DIR)/sendFileExplorerKeys.cpp \
          $(KEYBINDS_DIR)/sendSearchKeys.cpp \
          $(KEYBINDS_DIR)/sendGrepKeys.cpp \
          $(KEYBINDS_DIR)/sendCommandLineKeys.cpp \
          $(KEYBINDS_DIR)/sendShortcutKeys.cpp \
          $(KEYBINDS_DIR)/sendMultiCursorKeys.cpp \
          $(KEYBINDS_DIR)/sendTypingKeys.cpp

OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)
EXECUTABLE := $(BUILD_DIR)/e

.PHONY: all clean
all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/util $(BUILD_DIR)/keybinds

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

format:
	find . -iname *.cpp -o -iname *.h | xargs clang-format -i

test:
	make all && build/e longtest.js

install:
	make all && sudo mv build/e /usr/local/bin

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

.SECONDEXPANSION:
