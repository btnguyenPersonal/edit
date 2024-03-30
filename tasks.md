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

## TODO
- have cib ciB work for multi-line brackets
- ci' parse entire line to get actual strings instead of just between '
- ciw dot command not working correctly end string
- when not in typing restrict to have cursor not pass last character
- backup file (some way to debug??)
- add fuzzing

## LOW PRIO
- ! executes all current file in bash and replaces with output
- read from stdinput when just have dash
- add git gutter maybe () can jump to next/prev git changes
- handle render wide characters fully
- speed up grepFiles
    - use boyer-moore algorithm
        - https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm
    - figure out why fs::relative takes so damn long
        - should figure out a function to just cut off the parents/string at the point where we don't want, but every time i did it, it got slower
- make getch() int, and have enums for characters (this one is cancer to deal with, makes esc so laggy)
- research adding lsp??
- fileExplorer (current one is trash and i never use it) actually make it good now
