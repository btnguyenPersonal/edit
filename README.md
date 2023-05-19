# edit

this is my experiment for making a new code editor

# installing
```
npm i
npm run build
npm i -g .
```
that's it, you should just be able to run `edit file.js` in your command line and it should work!

# keybinds
### vim mode (starts in this mode and has all the shortcuts)
|key|function|
|:---:|:---:|
|h or left|move left one|
|j or down|move down one|
|k or up|move up one|
|l or right|move right one|
|u|undo|
|ctrl r|redo|
|w|jump forward one word|
|b|jump backward one word|
|x|delete character under cursor|
|i|go to normal mode|
|a|go to normal mode after cursor|
|I|go to normal mode at start of line|
|A|go to normal mode at end of line|
|ctrl u|go up half page|
|ctrl d|go down half page|
|P|paste before cursor|
|p|paste after cursor|
|gg|top of file|
|G|bottom of file|
|o|create new line under current line and go to normal mode|
|O|create new line above current line and go to normal mode|
|$|go to end of line|
|0|go to beginning of line|
|^|go to first word on line|
|v|enter visual mode (highlighting text)|
|V|enter visualLine mode (highlighting entire lines)|
|D|delete rest of current line|
|C|delete rest of current line and go to normal mode|
|S|turn current line into empty space and go to normal mode|
|J|join next line and current line|
|z|center screen|
|&#92;|reload file (if external program has changed it)|
|.|repeat last command|
|=|format indent for current line|
|m|mark line in file|
|'|return to mark|
|<>|change indent level|
|{}|find next/prev empty lines|
|[]|find next/prev lines with current indent level|
|()|find next/prev lines with lesser indent level|
|/|search in file, have to press enter to exit|
|*|search for word under cursor|
|n|repeat last search|
|N|repeat last search backwards|
|q|start/stop recording macro|
|,|repeat macro|
|@|repeat macro on every search result|
|r|replace current character with next specified character|
|f|jumps to next specified character|
|F|jumps to next specified character backwards|
|t|jumps right before next specified character|
|T|jumps right before next specified character backwards|
|cc|delete current line and go to normal mode|
|cG|delete until end of file and go to normal mode|
|cgg|delete until beginning of file and go to normal mode|
|cj|delete current line and down one and go to normal mode|
|ck|delete current line and up one and go to normal mode|
|ciw|delete current word and go to normal mode|
|cib or ci( or ci)|delete inside of rount brackets and go to normal mode|
|ciB or ci{ or ci}|delete inside of curly brackets and go to normal mode|
|ci[ or ci]|delete inside of square brackets and go to normal mode|
|ci' or ci"|delete inside of quotes and go to normal mode|
|cf|delete until next specified character and go to normal mode|
|cF|delete backward until next specified character and go to normal mode|
|ct|delete until right before next specified character and go to normal mode|
|cT|delete backward until right before next specified character and go to normal mode|
|dd| basically can do exact same things with c, but doesn't go to normal mode|
|yy| basically can do exact same things with c and d, but copies the text instead|
|-1234| jumps to line 1234, have to press enter to exit|
|ESCAPE| quits out of current keys for example typing in "d ESCAPE j" would stop d and just go down one|
|gc|comment / uncomment line|
|gt|trim trailing whitespace|

### normal mode (like a normal keyboard)
|key|function|
|:---:|:---:|
|up down left right|movement|
|backspace delete|deleting|
|ctrl w|deleting until previous word|
|ctrl a|go to start of words in line|
|ctrl e|go to end of line|
|ctrl s|save (should auto save for you tho)|
|tab and shift tab|increase/decrease indent|
|ESCAPE|go to vim mode|

### visual / visualLine mode (highlighting)
|key|function|
|:---:|:---:|
|up down left right hjkl w b ctrl u ctrl b| basically all the movement keys work the same|
|c| delete and go to normal mode|
|x| delete|
|d| copy and delete|
|y| copy|
|p| delete and paste|
|gc| comment / uncomment|
|<>| change indent|
|ESCAPE|go to vim mode|

# quality of life scripts
usually put this in my ~/.bashrc:
> this will let you use ctrl-p to change files and ctrl-g to search in project
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
            --bind "enter:execute(echo {1} {2} {q})+abort");
    until file=$(echo "$command" | cut -d' ' -f1-2) query=$(echo "$command" | cut -d' ' -f3-) && edit $command; do
        if [[ $? -eq 1 ]]; then
            clear;
            command="`fzf --reverse` 0 $query";
            if [[ "$(echo "${command}" | awk '{print $1}')" == "0" ]]; then
                command="$file $query"
            fi
        else
            command=$(rg --color=always --column --line-number --no-heading --smart-case "${*:-}" |
                fzf --ansi \
                    --bind "change:reload:sleep 0.1; rg --column --line-number --no-heading --color=always --smart-case {q} || true" \
                    --query "$query" \
                    --delimiter : \
                    --reverse \
                    --preview 'batcat --color=always {1} --highlight-line {2}' \
                    --preview-window 'up,60%,border-bottom,+{2}+3/3,~3' \
                    --bind "enter:execute(echo {1} {2} {q})+abort");
            if [ -z "${command// }" ]; then
                command="$file $query"
            fi
        fi
    done;
}
function e() {
    command="`fzf --reverse` 0 $query"
    until file=$(echo "$command" | cut -d' ' -f1-2) query=$(echo "$command" | cut -d' ' -f3-) && edit $command; do
        if [[ $? -eq 1 ]]; then
            clear;
            command="`fzf --reverse` 0 $query";
            if [[ "$(echo "${command}" | awk '{print $1}')" == "0" ]]; then
                command="$file $query"
            fi
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
            if [ -z "${command// }" ]; then
                command="$file $query"
            fi
        fi
    done;
}
export FZF_DEFAULT_COMMAND='rg --files --hidden --glob "!.git"'
```
