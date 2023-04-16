# edit

this is my experiment for making a new code editor

## backlog
    - undo/redo limit and diff based (don't know how hard that will be)
    - terminal instance inside editor
    - ability to change file in editor // want to put some work to make this part really nice, might come after opening as workspace
    - file explorer
    - change to using built in GUI stuff? might not bother with this one and do it myself
    - search and replace (file)
    - search in project
    - syntax highlighting
    - multithread stuff
    - split apart code into multiple files
    - after mouse scrolling so cursor isn't on screen, make sure next action resets the window // add to renderScreen
    - store cursor positioning between sessions
    - gcc commenting
    - tabs
    - zz
    - visual block
    - each mode in own file
    - clipboard stuff put into helper file
    - auto save?
    - w goes to ) as well for params?
    - visual deletes don't move windowLine
    - visual dot commands moving cursor in weird places
    - don't store col and row with clipboard
## todo
    - t T
    - f F
    - / ? n N // way to store & search for stuff (maybe make menu and have search/replace too?)
    - ci[
    - { }
    - add tests
        - mouse inputs done (might need to add drag select later)
        - helper functions for terminal done
        - normal keys
        - vim keys (bruh)
    - ( ) -> get out of current indent level brace / paren and start at ^
## blocked
## in progress
    - add emacs commands for insert mode
## done
    - disable blink or on click make cursor on
    - J
    - ci" ci' cib ciB
    - .
    - save indicator
    - make sure screen doesn't break when error happens
    - dot command
    - clipboard sync with system clipboard
    - d D
    - c C
    - p P
    - v paste
    - ciw
    - cc indentation
    - tab when o and O & enter
    - visual
    - clipboard
    - tab with > <
    - initial commit hello world
    - catch terminal and keystrokes
    - render file
    - save
    - insert and delete
    - make cursor and scrolling
    - put in line numbers
    - make sure only writable characters can be written (and tab)
    - fixed screen rendering being choppy
    - visual highlighting (not V or <c-v>)
    - cleaned cli inputs
    - highlight and copy/paste
    - on resize, reset window

## journal
    def terminal only
    going to fork vscode and see if I can just remove the stuff I don't want
        - bruh that shit is so complicated
    start with just single window with move around, insert characters, delete, save
    good progress first day, `terminal-kit` library is awesome, much easier than that library for rust apps
    making this is pretty fun, steady progress when I get on, feels like an actual editor right now, although lots of it sucks
