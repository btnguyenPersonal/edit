## TODO

- fileExplorer issues

- lock down changing files flag
	- turn on when unsaved??

- look at how hard it would be to have an icon at top to say there are new changes available?
	- each action check? if new changes happen have a popup like vim??

- fix gj gk same as vim

- use as mutt email maker (figure out how that works)
	- also remember to add muttrc to dotfiles

- add args
	- turn off autosave cmd
	-c "commands" (need to make parser for <c-h> <cr> etc)
	--help
	-h

- add gitlab and local server as mirror

- fix editorconfig filename parser

- put asserts everywhere (make sure state is perfect and i never mess up)

- fix diff function for huge files (look at vim, rope, other stuff)
	- futures api probably bad, use <pthread> semaphores and locks
	- do this with tests!!
	- have specific timestamps, and make a logging function that shows what happened when (with locks)
