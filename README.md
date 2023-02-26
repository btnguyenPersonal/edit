# rotide

this is my experiment for making a new code editor

## todo
    - add tests
    - after mouse scrolling so cursor isn't on screen, make sure next action resets the window
    - on resize, reset window
    - split apart code into multiple files
    - ability to change file
## blocked
    - fix flickering screen (maybe hold current screen string and updated screen string, go one by one and replace)
        - made some progress but only with some commands (renderScreen still causes big upsets, but can't be bothered right now)
        - tried putting empty strings after everything, but seemed very jank
        - next time try the hold previous state approach
## in progress
    - put in vim keybindings
## done
    - initial commit hello world
    - catch terminal and keystrokes
    - render file
    - save
    - insert and delete
    - make cursor and scrolling
    - put in line numbers
    - make sure only writable characters can be written (and tab)
## backlog
    - terminal instance inside editor
    - file explorer
    - change to using built in GUI stuff? might not bother with this one and do it myself
    - search and replace (file)
    - search in project
    - undo/redo
    - syntax highlighting
    - multithread stuff


## journal
    def terminal only
    going to fork vscode and see if I can just remove the stuff I don't want
        - bruh that shit is so complicated
    start with just single window with move around, insert characters, delete, save
    good progress first day, `terminal-kit` library is awesome, much easier than that library for rust apps
    making this is pretty fun, steady progress when I get on, feels like an actual editor right now, although lots of it sucks
