# edit

this is my experiment for making a new code editor

# installing
```
npm i
npm run build
npm i -g .
```
that's it, you should just be able to run `edit file.js` in your command line and it should work!

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
            command="`fzf --reverse` $query";
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
    command="`fzf --reverse` $query"
    until file=$(echo "$command" | cut -d' ' -f1-2) query=$(echo "$command" | cut -d' ' -f3-) && edit $command; do
        if [[ $? -eq 1 ]]; then
            clear;
            command="`fzf --reverse` $query";
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
