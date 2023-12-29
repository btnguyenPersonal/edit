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
- # pragma once?
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
