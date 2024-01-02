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
- what is good practice for `auto`? what about uint vs int?

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
