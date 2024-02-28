<<<<<<< Updated upstream
CC = g++
CFLAGS = -Wall -lncurses
=======
CC = g++-13
CFLAGS = -Wall -lncurses -pthread
>>>>>>> Stashed changes
DEPFLAGS = -MMD -MP

SRC_DIR = src
KEYBINDS_DIR = $(SRC_DIR)/keybinds
UTIL_DIR = $(SRC_DIR)/util
BUILD_DIR = build

SOURCES = $(SRC_DIR)/edit.cpp \
          $(UTIL_DIR)/helper.cpp \
          $(UTIL_DIR)/render.cpp \
          $(UTIL_DIR)/state.cpp \
          $(UTIL_DIR)/clipboard.cpp \
          $(UTIL_DIR)/history.cpp \
          $(UTIL_DIR)/indent.cpp \
          $(UTIL_DIR)/comment.cpp \
          $(UTIL_DIR)/insertLoggingCode.cpp \
          $(KEYBINDS_DIR)/sendKeys.cpp \
          $(KEYBINDS_DIR)/sendVisualKeys.cpp \
          $(KEYBINDS_DIR)/sendBlameKeys.cpp \
          $(KEYBINDS_DIR)/sendFindFileKeys.cpp \
          $(KEYBINDS_DIR)/sendSearchKeys.cpp \
          $(KEYBINDS_DIR)/sendGrepKeys.cpp \
          $(KEYBINDS_DIR)/sendCommandLineKeys.cpp \
          $(KEYBINDS_DIR)/sendShortcutKeys.cpp \
          $(KEYBINDS_DIR)/sendMultiCursorKeys.cpp \
          $(KEYBINDS_DIR)/sendTypingKeys.cpp

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

DEPS = $(OBJECTS:.o=.d)

EXECUTABLE = $(BUILD_DIR)/e

all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/util $(BUILD_DIR)/keybinds

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

test:
	make all && build/e longtest.md

install:
	make all && sudo cp build/e /usr/local/bin

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

.PHONY: all clean
