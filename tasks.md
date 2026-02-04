## tasks

- more tests
	[x] string
	[x] autocomplete
	[x] save
	[x] read
	[x] sanity
	[x] typingKeys
	[x] insertLoggingCode
	[ ] fileExplorerNode <- have moved up higher because fuzzing has proven them buggy
	[ ] fileExplorerKeys
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
	[ ] multicursorKeys
	[ ] normalKeys

perf:
	- use trace() to see all the ncurses calls
	- multiline comments render fast
	- very long lines render fast
	- paste into terminal fast dont have to use gp gy
	- speed up history somehow, takes even longer for large files

crash:
	- ctrl r crash
	- wrap all the places we use std::filesystem in try/catch

- getColForDisplay threadsanitize

improve autocomplete:
	- autocomplete allow pressing tab multiple times
	- maybe have autocomplete also scan the other harpooned files?

don't move file explorer cursor all the time, and just press = to refocus

Use  the  addchstr()  family  of functions for fast screen-painting of
   text  when  you  know the text doesn't contain any control characters.
   Try  to  make  attribute changes infrequent on your screens. Don't use
   the immedok() option!

use getnstr(); << don't know how this works with mouse input
(maybe do some kind of test to see if there is two inputs, then use ungetch and start using getnstr() <- note backspace will work with this)

copy/paste to system clipboard in background

get multiline cached and only recalculated every second

get veryvery long line editing working

remove ncurses capturing all keyboard input, why is it so slow?? is that as fast as it goes??
- faster way to do input, needs to work for pasting a million lines

make word wrap work well with tabs when windowPos.col > 0
- make a windowLineCounter when rendering and just start at 0 and have a renderPixels cut off before x amount

clean up paste functions in clipboard

repeat number keys

W and B

ts-server integration (do it for real this time)

instead of comparing with the previousState and generating the diff every time, could compute the state->file->data hash and store that
if could get all of the edits to instead of use raw std::string, go through some abstraction layer, could make undo and file hash compute
way easier
- using ratchet will make this better
