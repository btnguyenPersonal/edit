## tasks

- more tests
	[x] string
	[x] autocomplete
	[x] save
	[x] read
	[x] sanity
	[x] typingKeys
	[x] insertLoggingCode
	[ ] render
	[ ] movement
	[ ] clipboard
	[ ] fileops
	[ ] getBounds
	[ ] comment
	[ ] indent
	[ ] display
	[ ] external
	[ ] harpoon
	[ ] keys
	[ ] grep
	[ ] find
	[ ] cleanup
	[ ] dirSplit
	[ ] history
	[ ] visual
	[ ] textedit
	[ ] search
	[ ] repeat
	[ ] fileExplorerNode
	[ ] fileExplorerKeys
	[ ] multicursorKeys
	[ ] normalKeys

compat:
	- use cursor as invisible with inverted
	- mouse
	- replace state->maxX state->maxY with LINES and COLUMNS

perf:
	- use trace() to see all the ncurses calls
	- multiline comments render fast
	- very long lines render fast
	- paste into terminal fast dont have to use gp gy
	- speed up history somehow, takes even longer for large files

crash:
	- ctrl r crash

improve autocomplete:
	- autocomplete allow pressing tab multiple times
	- maybe have autocomplete also scan the other harpooned files?

improve build time! it is unbearable!!

	- build as one translation unit
	- see what dependencies are bloating it out & replace
		- ncurses
		- string
		- vector
		- fstream
		- algorithm
		- climits
		- cstdint
		- map
		- mutex
	- compiler flags turn off exceptions
	- more compiler flags
	- remove make
	- performance indicators

repeat number keys

W and B

ts-server integration (do it for real this time)

instead of comparing with the previousState and generating the diff every time, could compute the state->file->data hash and store that
if could get all of the edits to instead of use raw std::string, go through some abstraction layer, could make undo and file hash compute
way easier
