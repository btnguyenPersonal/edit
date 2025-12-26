## tasks
use these to fix problems
-fsanitize=thread
	- to the best of my noob brain what's happening is that everything is fine with the actual grep and find stuff happening, it's just that the extra renderScreen(state); that is async as well is the big problem with my current setup. It looks like I will have to either guard every state access with renderMutex, or re-architect how those calls are happening so that the renderScreen doesn't have to be async. Maybe it could be something like setting a flag somewhere (something better than state->shouldReRenderWhenPossible), and have the main key loop have an else that looks at that, and just calls renderScreen only in that case (also just fixed that shouldReRender thing and something's still happening, will look tomorrow)

-fsanitize=undefined -fno-sanitize-recover=all

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
