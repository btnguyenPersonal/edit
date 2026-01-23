# TextBuffer Interface Documentation

## Overview
The TextBuffer abstraction layer provides a clean separation between read-only and write operations over `state->file->data`. This enables centralized control, validation, and caching opportunities.

## Read-Only Operations
These operations **never modify** the buffer and are safe to use for rendering, analysis, and validation.

### Character Access
```cpp
char textbuffer_getChar(State *state, uint32_t row, uint32_t col);
```
- Returns character at position or '\0' if invalid

### Line Access
```cpp
const std::string &textbuffer_getLine(State *state, uint32_t row);
```
- Returns const reference to line content
- Safe for read-only operations

### Utility Operations
```cpp
uint32_t textbuffer_getLineCount(State *state);
uint32_t textbuffer_getLineLength(State *state, uint32_t row);
bool textbuffer_isValidPosition(State *state, uint32_t row, uint32_t col);
const std::vector<std::string> &textbuffer_getLines(State *state);
```
- Non-modifying queries for buffer information
- Safe for rendering and navigation

## Write Operations
These operations **modify** the buffer and trigger cache invalidation and history tracking.

### Character Operations
```cpp
void textbuffer_insertChar(State *state, uint32_t row, uint32_t col, char c);
void textbuffer_deleteChar(State *state, uint32_t row, uint32_t col);
void textbuffer_replaceChar(State *state, uint32_t row, uint32_t col, char c);
```
- Modify individual characters
- Automatic bounds checking
- Cache invalidation triggered

### Line Operations
```cpp
void textbuffer_insertLine(State *state, uint32_t row, const std::string &line);
void textbuffer_deleteLine(State *state, uint32_t row);
void textbuffer_replaceLine(State *state, uint32_t row, const std::string &newLine);
void textbuffer_splitLine(State *state, uint32_t row, uint32_t col);
void textbuffer_joinLines(State *state, uint32_t row1, uint32_t row2);
```
- Modify line structure
- `textbuffer_replaceLine` is the **preferred method** for single-line replacements
- More efficient than `textbuffer_replaceRange` for single lines

### Range Operations
```cpp
void textbuffer_insertRange(State *state, uint32_t row, const std::vector<std::string> &lines);
void textbuffer_deleteRange(State *state, uint32_t startRow, uint32_t endRow);
void textbuffer_replaceRange(State *state, uint32_t startRow, uint32_t endRow, const std::vector<std::string> &lines);
```
- Bulk operations for multiple lines
- Optimized for visual mode operations

## Benefits

### 1. Clear Intent Separation
- **Read-only** functions cannot accidentally modify data
- **Write** functions clearly indicate modification intent
- Compile-time safety for operation types

### 2. Centralized Validation
- All operations go through the same validation logic
- Consistent bounds checking and error handling
- Debug assertions for development

### 3. Cache Integration
- Write operations automatically trigger `textbuffer_invalidateCache()`
- Read operations can safely use cached data
- Hook points for future optimization

### 4. History Tracking
- Write operations automatically call `textbuffer_markModified()`
- Centralized history position management
- Ready for undo/redo integration

### 5. Performance Optimization
- Read operations avoid unnecessary overhead
- Write operations batch cache invalidation
- Ready for future caching optimizations

## Usage Examples

### Reading Line Content (Safe)
```cpp
// Good - const reference, no copy
const std::string &line = textbuffer_getLine(state, row);

// Good - for read-only character access
char c = textbuffer_getChar(state, row, col);

// Good - for bounds checking
if (textbuffer_isValidPosition(state, row, col)) {
    // Safe to access
}
```

### Modifying Line Content
```cpp
// Good - single line replacement
textbuffer_replaceLine(state, row, newContent);

// Good - character replacement  
textbuffer_replaceChar(state, row, col, 'x');

// Good - line splitting
textbuffer_splitLine(state, row, col);
```

### Complex Operations
```cpp
// Good - multi-line replacement
textbuffer_replaceRange(state, startRow, endRow, newLines);

// Good - range deletion
textbuffer_deleteRange(state, startRow, endRow);
```

## Migration Status
- ✅ **sendTypingKeys.cpp** - Fully migrated
- ✅ **sendNormalKeys.cpp** - Fully migrated  
- ✅ **sendVisualKeys.cpp** - Fully migrated
- 🔄 **Other modules** - Ready for migration

## Future Enhancements
1. **Caching Layer** - Line content, syntax highlighting, search indexes
2. **Performance Monitoring** - Operation timing and profiling
3. **Batch Operations** - Bulk write operations with single invalidation
4. **Memory Optimization** - Object pooling and arena allocation