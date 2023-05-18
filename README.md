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
### vim mode
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
|\|reload file (if external program has changed it)|
|.|repeat last command|
|=|format indent for current line|
|m|mark line in file|
|'|return to mark|
|<>|change indent level|
|{}|find next/prev empty lines|
|[]|find next/prev lines with current indent level|
|()|find next/prev lines with lesser indent level|
|/|search in file|
|*|search for word under cursor|
|n|repeat last search|
|N|repeat last search backwards|
|q|start/stop recording macro|
|f<insert character here>|jumps to next specified character|
|F<insert character here>|jumps to next specified character backwards|
|t<insert character here>|jumps right before next specified character|
|T<insert character here>|jumps right before next specified character backwards|
|,|repeat macro|
|@|repeat macro on every search result|

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
            if [ -z "${command// }" ]; then
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
            if [ -z "${command// }" ]; then
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
