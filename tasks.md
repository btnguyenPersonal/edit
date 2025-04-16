## TODO

- fix blame getting calculated every render (slowing down fuzzing a lot)

- config based on filetype
  - would probably have to re-interpret config every file change bleh
  - also needs to add glob matching and the entire spec gross

- indent_style tab or space
    - would probably need a :retab command (find all preceding spaces of state->indent length and convert to tab)

- put asserts everywhere (make sure state is perfect and i never mess up)

- fix centering screen when j or k on wrapped line

- fix visual going up and indenting repeated

- fix diff function for huge files (look at vim, rope, other stuff)

- go line by line, and do a code review of everything on github (especially the llm crap i have in here)

## BRAINSTORMING

- what about ertwq as camera keys?? they could be the exact same as starcraft, maybe?? would be able to get rid of completely all parody, think about consequences:
    q - macro
        - ez replace w/ anything
    w - forward word
        - hardest one to remap by far
        - ideas?:
            - could try just using f keys
            - W?
            - also want this to be a one-button guy
    e - comment
        - could muscle memory to ; ?? gc?? E?? really want this to be a one button no modifiers key
        - dash key?? (-)
    r - replace word
        - could muscle memory to vrX
    t - forward to x
        - don't use really in normal mode

    ctrl q - undo jumpList
        - didn't even remember this was a thing
    ctrl w - backwards harpoon
        - will be unnecessary
    ctrl e - forwards harpoon
        - will be unnecessary
    ctrl r - redo
        - hard to rebind honestly, maybe ctrl y? ctrl l? U? <- remember to fix all the special ctrl('r') logic
    ctrl t - fileexplorer
        - will also be hard to rebind (ctrl h or ctrl l??) < prob ctrl h
