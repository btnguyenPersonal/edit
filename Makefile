CC = g++
CFLAGS = -Wall -lncurses

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
          $(KEYBINDS_DIR)/sendKeys.cpp \
          $(KEYBINDS_DIR)/sendVisualKeys.cpp \
          $(KEYBINDS_DIR)/sendFindFileKeys.cpp \
          $(KEYBINDS_DIR)/sendSearchKeys.cpp \
          $(KEYBINDS_DIR)/sendGrepKeys.cpp \
          $(KEYBINDS_DIR)/sendCommandLineKeys.cpp \
          $(KEYBINDS_DIR)/sendShortcutKeys.cpp \
          $(KEYBINDS_DIR)/sendTypingKeys.cpp

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

EXECUTABLE = $(BUILD_DIR)/e

all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/util $(BUILD_DIR)/keybinds

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

install:
	make all && sudo cp build/e /usr/local/bin

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
