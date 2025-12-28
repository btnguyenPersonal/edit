## tasks
- improve build time! it is unbearable!!

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

- more tests

- editing new file indicator [NEW] on right side??

- repeat number keys

- W and B

- ts-server integration (do it for real this time)

instead of comparing with the previousState and generating the diff every time, could compute the state->file->data hash and store that
if could get all of the edits to instead of use raw std::string, go through some abstraction layer, could make undo and file hash compute
way easier
