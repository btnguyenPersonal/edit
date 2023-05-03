# edit

this is my experiment for making a new code editor

usually put this in my ~/.bashrc:
> use e to make <ctrl-p> to change files and <ctrl-g> to search in project
```
function g() {
    command=$(rg --color=always --column --line-number --no-heading --smart-case "${*:-}" |
        fzf --ansi \
            --bind "change:reload:sleep 0.1; rg --column --line-number --no-heading --color=always --smart-case {q} || true" \
            --query "$query" \
            --delimiter : \
            --reverse \
            --preview 'batcat --color=always {1} --highlight-line {2}' \
            --preview-window 'up,60%,border-bottom,+{2}+3/3,~3' \
            --bind "enter:execute(echo '{1} {2} {q}')+abort");
    until query="${command##* }" && edit $command; do
        if [[ $? -eq 1 ]]; then
            clear;
            command="`fzf --reverse` $query";
        else
            command=$(rg --color=always --column --line-number --no-heading --smart-case "${*:-}" |
                fzf --ansi \
                    --bind "change:reload:sleep 0.1; rg --column --line-number --no-heading --color=always --smart-case {q} || true" \
                    --query "$query" \
                    --delimiter : \
                    --reverse \
                    --preview 'batcat --color=always {1} --highlight-line {2}' \
                    --preview-window 'up,60%,border-bottom,+{2}+3/3,~3' \
                    --bind "enter:execute(echo '{1} {2} {q}')+abort");
        fi
    done;
}
function e() {
    command="`fzf --reverse --height=8` $query"
    until query="${command##* }" && edit $command; do
        if [[ $? -eq 1 ]]; then
            clear;
            command="`fzf --reverse --height=8` $query";
        else
            command=$(rg --color=always --column --line-number --no-heading --smart-case "${*:-}" |
                fzf --ansi \
                    --bind "change:reload:sleep 0.1; rg --column --line-number --no-heading --color=always --smart-case {q} || true" \
                    --query "$query" \
                    --delimiter : \
                    --reverse \
                    --preview 'batcat --color=always {1} --highlight-line {2}' \
                    --preview-window 'up,60%,border-bottom,+{2}+3/3,~3' \
                    --bind "enter:execute(echo '{1} {2} {q}')+abort");
        fi
    done;
}
export FZF_DEFAULT_COMMAND='rg --files --hidden --glob "!.git"'
```

## backlog
    - undo/redo limit and diff based (don't know how hard that will be)
    - file explorer
    - multithread stuff
    - split apart code into multiple files
    - store cursor positioning between sessions
    - visual block
    - <c-o> <c-i>
    - ctrl w and ctrl backspace
    - trim trialing space on save and update state
    - habit: use cib for params
    - syntax highlighting for different types of files
    - get rid of process.stdout.row and use screen.height screen.width (will make easier to make status bar)
## todo
    - add status bar
    - mouse drag select
    - add tests
        - mouse inputs done (might need to add drag select later)
        - helper functions for terminal done
        - normal keys
        - vim keys (bruh)
## blocked
    - deal with wrapping lines // lol just don't worry about it
    - until status bar >> do i really need?
        - show file path
        - show which mode are in
        - show search string
        - show search result 5/60
        - show if search is wrong
## in progress
    - grepping project (need to keep the previous text searched)
## done
    - terminal instance inside editor
    - ability to change file in editor // want to put some work to make this part really nice, might come after opening as workspace
    - change to using built in GUI stuff? might not bother with this one and do it myself
    - actual copy paste newLine or not detection
    - ( ) -> get out of current indent level brace / paren and start at ^
    - q macros
    - fix bug w/ eof using Vd
    - visual deletes don't move windowLine
    - gg command? dot
    - after mouse scrolling so cursor isn't on screen, make sure next action resets the window // add to renderScreen
    - search and replace (file)
    - syntax highlighting
    - gcc commenting
    - tabs
    - zz
    - each mode in own file
    - clipboard stuff put into helper file
    - auto save?
    - visual dot commands moving cursor in weird places // maybe search replace k with j and start at top row
    - don't store col and row with clipboard
    - gg not working right
    - priming function for the snapshots? take row col from before edit
    - Vc not working
    - delete for /
    - save point changing snapshot for isSaved
    - / ? n N // way to store & search for stuff (maybe make menu and have search/replace too?)
        - / sets to slash mode
        - type whatever searches (ignorecase)
        - enter stores search to state (state.search)
            - also sets visual to next occurence (will need some error message if can't find, and some status bar showing number of occurences (6/15) and searched phrase)
            - r to replace current, R to replace all, n next, N backwards
            - somehow make a search that is project wide, is that what ? could be??
        - will need to have some kind of highlighting all occurences while in the special visual-search state?
    - cursor freakout
    - gg on V
    - G and gg on dot
    - improve render a lot to not rerender, and save lines for the color instead of by char
    - gc deleting when //comment is close
    - commenting
    - add emacs commands for insert mode
    - t T //need c and d to work with this -> seems like should be able to convert this somehow to simplify logic?
    - f F
    - { }
    - ci[
    - cj ck
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
