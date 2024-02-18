# edit-c++
trying my hand at cpp to make a term-based code editor

# day 0
have no idea how to even read file in (i'm so rusty)
got bones working to start displaying file

# TODO for working self-editing:
- save file
- typing mode
- cursor at right position during render
- window auto focusing the cursor
- tabs prolly?

# so I don't hate myself
- Makefile that isn't dumb
- copy paste

# day 1
- got typing and moving around kinda working, still can't edit files bigger than term length
- got color sorta working, but not all 256 yet
- moved around file structure and Makefile compiling .cpp and .h -> .o -> binary
- tried for an hour getting clang-tidy linter set up; why is it so damn hard?
  - bruh have to wrap the compile command with `bear` and then generate `compile_commands.json`

# questions about c++
- pragma once?
- are linters a thing?
- how do you run tests?
- what is good style to put in .h vs .cpp?
- what are good CFLAGS?
- where should you put .o and binary?
- is it good style to put `using namespace std`?
- should i learn to use char* or string?
- default arrays or vector?
- should you always free everything before returning from main?
- is there a way to find out if any of my imports are not being used?
- what is good practice for `auto`? what about unsigned int vs int?

# day 2-3
- got motions set up, much cleaner than edit1.0
- have copy paste working with system paste, just need to make a paste shortcut and set up logic to paste into current file
- this time wanna put all HIGHLIGHTING into one file (maybe another enum specifying what type of HIGHLIGHTING we are in(v ctrLv or V)
- cursor is fully working, and logic is better because there is a sanity check every render, and no weird edge cases
- found out that terminal window size detection actually works unlike js
- got a shower thought that for big files want to do DoubleLinkedList<DoubleLinkedList> data, because for inserting and deleting would be faster
  - will have to test performance, but inserting be O(1) should make things a lot faster I would think, because that's almost all the actions going on
- I wonder how bad perf is right now for >1GB files?

```
line 0 | 'H' <-> 'E' <-> 'L' <-> 'L' <-> 'O'
line 1 | 'W' <-> 'O' <-> 'R' <-> 'L' <-> 'D' <-> '!'
line 2 | ''

0 <-> 1 <-> 2
```

# day 4ish
- colors!!
- made render function amazing, can deal with files that are 256000 lines long comfortably
- omg pointers make everything so fast, didn't know that putting something in function params made a copy neat
- putting a huge file in made a huge difference for finding bugs
- maybe later will test with even larger files
- want to start self editing, but at least need
    - visual mode
    - diff based undo
    - commenting
    - searching

    - autosave feels really nice
    - tab completion
    - file tabs
    (wanna try making these fullscreen ones off to the side maybe?? atleast fileexplorer should be)
    - grep
    - filefinder
    - fileexplorer

# day 5ish
- added visualLine
- wow much cleaner than the js crap I had
- std::vector has cool iterators
- had to add the same if (deleted entire file) { add empty line }
- also black seems a bit different color but idk
- plan for tomorrow:
    - paste
    - visual (normal)
    - visual (block)
    - commenting
- plan for sunday:
    - undo (diff based) don't chicken out again I tried in edit1.0 like 2 times and failed

# weekend over
- paste working, uses newline at end like correct
- visualLine working, need visual and visualBlock
- wow why was undo so hard before?
- i mean i didn't get the most efficient one, but whatever I'll fix it later when i need perf
- undo also why didn't i just find the location change happened instead of storing it with the diff???

# switch over
- let's see how this goes, hopefully no bugs pop up fingers crossed


# grep & findfile working
todo need harpoon(need to figure out how to store old states, maybe make an archive? maps filename to old state)
search replace

??? todo make H character switch between .cpp and .h

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

## TODO
- clean up printChar
- make getch() int, and have enums for characters
- indent w/ html
- indent w/ blank lines
- render non-alphanumeric chars
- mulit-cursor mode
- ctrl w when typing
- put handleMotion at top, and only look at state->motion
- clean up handleMotion
- make all use handleMotion
- check for 256 colors, and if not go back to default colors (IFDEF something)

## LOW PRIO
- fix handle motion not clearing motion if first one (and make everything a motion even single ones);
- fix indent for commented lines at end
- fix indent for dealing with spaces
- cursor for ctrl p ctrl g
- fileExplorer (current one is trash) actually make it good now
- ctrl a x increment decrement
- J K (only base indent off of first line
- turn off isInString highlighting for md txt files
- gr -> go to top and find first occ w/ import in front -> go to file at top -> set searchQuery to it -> search for it
- make row col, etc all Position types like visual
- figure out sendShortcutKeys not repeat a bunch of visual shortcuts

