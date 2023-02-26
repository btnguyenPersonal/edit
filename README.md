# rotide

this is my experiment for making a new code editor

## first thought
    def terminal only
    going to fork vscode and see if I can just remove the stuff I don't want
        - bruh that shit is so complicated
    start with just single window with move around, insert characters, delete, save
    good progress first day, `terminal-kit` library is awesome, much easier than that library for rust apps

## todo
    - put in vim keybindings
## blocked
    - fix flickering screen (maybe hold current screen string and updated screen string, go one by one and replace)
        - made some progress but only with some commands
## in progress
## done
    - initial commit hello world
    - catch terminal and keystrokes
    - render file
    - save
    - insert and delete
    - make cursor and scrolling
    - put in line numbers
    - make sure only writable characters can be written (and tab)