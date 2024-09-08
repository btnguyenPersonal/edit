## DONE
- search replace highlighting, N
- J K
- visual > <
- if file not found guard against that
- horizontal scrolling
- ctrl o, i really
  - maybe have file stack on right side? might be cool
- \ reload file
- fix searching crashing everything (remake from scratch prob)
- ctrl h ctrl l move around harpoon
- getFromClipbpoard when nothing in clipboard
- dot command
- comment highlighting
- ignorelist actually work
- indenting
    - real time, never have to worry about breaking everything
    - will prob require a parser and need to keep a syntax tree in the state
    - I wonder if we can isolate each line, so only need to update one line of the tree
    - if that's not possible, just keep references in there, so only have to update all deps
        - (might be all lines after current line? if is don't bother with references);
    - how do I handle wrong indenations? thinking putting ghost red text after badly indented line and having red background line number
- start ctrl p with all highlighted
- gf
- commenting
- ctrl d - print console.log equivalent at start of line w/ line number (eg 0144 to make it easy to read)
- ctrl p only filter based on relative path instead of absolute
- gf work for js
- tab complete
- git precommit hook disable u i n t
- global search replace
- current line search replace w/ regex
- git blame mode (copy hash to clipboard)
- ctrl w if at end also redirect to current
- { } [ ] and skip whitespace
- * and # when in normal mode
- dt, df, t, f, T, F
- ip ap motions
- put handleMotion at top, and only look at state->motion
- clean up handleMotion
- make all use handleMotion
- clean up printChar
- render non-alphanumeric chars
- fix handle motion not clearing motion if first one (and make everything a motion even single ones);
- figure out sendShortcutKeys not repeat a bunch of visual shortcuts
- fix indent for commented lines at end
- fix indent for dealing with spaces
- indent w/ blank lines
- J K (only base indent off of first line)
- turn off isInString highlighting for md txt files
- check that isnt a folder when opening file state
- if no diff happens, but should update dot command, doesn't happen
- check for 256 colors, and if not go back to default colors (IFDEF something)
- ctrl w when typing
- kinda working cursor for ctrl p ctrl g (made global search replace easier to get to)
- find file copy relative path && absolute path
- multi-cursor mode
- make ctrl q put below line instead of on current line
- made findfile fzf
- figure out why sometimes the indent doesn't work with empty lines
- indent w/ html
    - might need to scan entire file to see if in html
        - could i treat middle of tag as non-html?:
            <div
                classname="one"
                id="two"
                label="three"
            >
        - could i treat middle of jsx as non-html?
            <div>
                {true ? (
                    <Table />
                ) : (
                    <Row />
                )}
            </div>
    - based on previous line and current line?
    - will have to go character by character and make a parser to see if all the closing/opening tags match up
- @ to get grepQuery to searchQuery
- global mark, go back to file and line
- better arg handling
- better error handling for block visual
- fix [] for commented lines
- fix visual paste when mixing LINE and NORMAL (eg. pasting line when in normal and pasting normal when in line)
- ctrl a ctrl s w/ g
- ctrl a s increment decrement
- sorting lines
- gr
    -> maybe just finding the topmost reference will be okay
    -> somehow go to the definition of the var/function, may have to use lsp somehow
    -> go to top and find first occ w/ import in front -> go to file at top -> set searchQuery to it -> search for it
- add %
- auto show matching parenthesis
- gu gU
- conflict markers
- search current line after grep
- backspace work for re-joining lines
- make apm counter change color based on how on fast it is
- ctrl v paste into grepQuery or findFile
- visual replace one char
- visual block join
- true jumplist
- run current search replace in visual
- do something with 1-9 (maybe repeat everything? n times)
- add fuzzing
- make getch() int, and have enums for characters
    - this one is cancer to deal with, makes esc so laggy
    - export ESCDELAY=0 ????? fixes everything
- add cursors that are movable for findfile, grep, etc as structs, and change all the state->grep crap to state->grep.query
- make search save queries
- read from stdinput when just have dash
- ! executes all current file in bash and replaces with output
- fix rendering colors
- (figure out how to abstract attron and attroff stuff (better color control around it))
- make helper function for printing w/ color w/o having to turn on and off manually duping logic before and after print
- clean up all the render code
- backup file (some way to debug?? dump entire state when segfault error happns like edit js had)
- logging file (per session log all keypresses, in what order so can do analysis on most used commands, etc)
- lots of valgrind errors?? need to figure out what's going on there
    - maybe should ask someone w/ more c++ logic where my deallocations should be, probably not right to just treat it as garbage collected
- figure out how to go down to assembly and view it
- fix history slowing down copy/pasting large diff (also look into what is really causing it)
    - also could hack into the std::string, add a modified value, use that to parse history
    - THE PLAN!!!!!!
    - we make one big for loop
        - it tries add and delete at the same time, and if it finds one it cuts everything off

## TODO
- add filetree on left side, that has to be toggled open/closed, and allows you to just click through all the files really quick to see which one you want
    - fix status bar abstraction for calculating where to render stuff on the status bar (offset stuff)
    - need to change
        - printLineNumber
        - printLine
        - renderStatusBar? - prob not see how it looks

        - render fileExplorer
            - prob start with it default open just to see improvements faster
            - cache explorer output when opening fileExplorer (start by gathering root directory, and listing only current files, and only explore down current file path)
                - show ignoreList as grey, but still show it
                - current as middle if needed
            - have everything default closed except top level files/folders (and show where current file is)
            - always center on current file when changing file
                - also when changing file don't autoclose, just open new path to new current file if needed
            - search also needs to work same (maybe refactor search to work generically??)
            - do i keep it open all the time?
                - maybe press esc to go back to editor, and ctrl z to close
            - open w/ enter, j, k, ctrl u, ctrl d, G, gg, z, ctrl r copy relative file path, R hard reset to only showing current file paths opened, have ctrl o goto parent
            - add copy/paste(folders too)/rename(with current file rename as well)
                - keybind: `I`: import files/folder from other project? somehow trigger os to open fileExplorer and select folder

- add go to definition (without any lsp will be really hard)
    - have strategies that you try each one, and if you find definition change to it
        - ex. see if current is in current file as const `match` = ...;
        - ex. see if current matches import { `match` } from '../../src';
        - ex. see if current is a function of an import

- ci' parse entire line to get actual strings instead of just between ' (maybe try out for a while)

- render test files in a different color, put them under source files

## BACKLOG
- add mouse clicks
- add mouse drag
- add mouse scroll
- add python formatting
- have cib ciB work for multi-line brackets
- ciw dot command not working correctly
- when not in typing restrict to have cursor not pass last character
- add multi-line searches
- need to fix the crap with onlyMotions, every motion should work all the same and not have jank logic (maybe separate out motions into it's own thing?)
    - have isMotion() can't be too broken?
    - maybe just having maps of keycombos and function pointers is the correct call??
- add git gutter maybe () can jump to next/prev git changes
- handle render wide characters fully
- speed up grepFiles
    - use boyer-moore algorithm
        - https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm
    - figure out why fs::relative takes so damn long
        - should figure out a function to just cut off the parents/string at the point where we don't want, but every time i did it, it got slower
- research adding lsp??
- fileExplorer (current one is trash and i never use it) actually make it good now
