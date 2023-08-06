# edit

this is my experiment for making a new code editor

# installing
```
npm i
npm i -g .
```
that's it, you should just be able to run `e path/to/file.js` in your command line and it should work!
also, just starting as `e` should start you out with searching for the file you want

### Idea for editor:
I don't like holding ctrl all the time to do shortcuts, and I was mostly just using shortcuts exclusively when editing code anyway

so now this editor switches between "modes"
- shortcut mode: basically all the shortcuts without having to hold ctrl
- typing mode: normal keyboard
- visual mode: highlighting stuff

example:

    ```
    function testing(hi) {
        int l;
        int y;
    }
    ```

- cut current line `dd`
    ```
    function testing(hi) {
        int y;
    }
    ```

- paste line under current line a couple of times `ppp`
    ```
    function testing(hi) {
        int y;
        int l;
        int l;
        int l;
    }
    ```

- go to top of file `gg`
    ```
    function testing(hi) {
        int y;
        int l;
        int l;
        int l;
    }
    ```

- change inside of current/next brackets `ci(` `helloworld!<esc>`
    - this does switch to typing mode for typing `helloworld!`, then goes back to shortcut mode by pressing escape
    ```
    function testing(helloworld) {
        int y;
        int l;
        int l;
        int l;
    }
    ```

# keybinds
### shortcut mode
- `escape`

|key|function|
|:---:|:---:|
|:q|quit|
|ctrl p|search for file in project|
|ctrl g|find occurrences of text in project|
|h or left|move left one|
|j or down|move down one|
|k or up|move up one|
|l or right|move right one|
|u|undo|
|ctrl r|redo|
|w|jump forward one word|
|b|jump backward one word|
|x|delete character under cursor|
|i|go to typing mode|
|a|go to typing mode after cursor|
|I|go to typing mode at start of line|
|A|go to typing mode at end of line|
|ctrl u|go up half page|
|ctrl d|go down half page|
|P|paste before cursor|
|p|paste after cursor|
|gg|top of file|
|G|bottom of file|
|o|create new line under current line and go to typing mode|
|O|create new line above current line and go to typing mode|
|$|go to end of line|
|0|go to beginning of line|
|^|go to first word on line|
|v|enter visual mode (highlighting text)|
|V|enter visualLine mode (highlighting entire lines)|
|D|delete rest of current line|
|C|delete rest of current line and go to typing mode|
|Y|copy rest of current line|
|S|turn current line into empty space and go to typing mode|
|J|join next line and current line|
|z|center screen|
|&#92;|reload file (if external program has changed it)|
|.|repeat last command|
|=|format indent for current line|
|ctrl w/e|switch between tabs|
|ctrl h/l|move tabs|
|space|toggle current file as a tab|
|X|clear tabs|
|ctrl x|remove current tab|
|m|mark line in file|
|'|return to mark|
|M|alt mark line in file|
|"|return to alt mark|
|<>|change indent level|
|{}|find next/prev empty lines|
|[]|find next/prev lines with current indent level|
|()|find next/prev lines with lesser indent level|
|/|search in file, have to press enter to exit, esc to cancel, ctrl f to start replacing|
|*|search for word under cursor|
|ctrl f|replaces all occurrences of your searched text|
|n|repeat last search down the file|
|N|repeat last search up the file|
|q|start/stop recording macro|
|,|repeat macro|
|r|replace current character with next typed character|
|f|jumps to next specified character|
|F|jumps to next specified character backwards|
|t|jumps right before next specified character|
|T|jumps right before next specified character backwards|
|cc|cut current line and go to typing mode|
|cG|cut until end of file and go to typing mode|
|cgg|cut until beginning of file and go to typing mode|
|cj|cut current line and down one and go to typing mode|
|ck|cut current line and up one and go to typing mode|
|ciw|cut current word and go to typing mode|
|cib or ci( or ci)|cut inside of rount brackets and go to typing mode|
|ciB or ci{ or ci}|cut inside of curly brackets and go to typing mode|
|ci[ or ci]|cut inside of square brackets and go to typing mode|
|ci' or ci"|cut inside of quotes and go to typing mode|
|cf|cut until next specified character and go to typing mode|
|cF|cut backward until next specified character and go to typing mode|
|ct|cut until right before next specified character and go to typing mode|
|cT|cut backward until right before next specified character and go to typing mode|
|dd| basically can do exact same things with c, but doesn't go to typing mode|
|yy| basically can do exact same things with c and d, but copies the text instead|
|1234| jumps to line 1234|
|ESCAPE| quits out of current keys for example typing in "d ESCAPE j" would stop d and just go down one|
|e|comment / uncomment line|
|ge|search for next commented block and uncomment it|
|gt|trim trailing whitespace|

### typing mode
- `i` `a` `I` `A` `O` `o` `c commands(see above)`
|key|function|
|:---:|:---:|
|up down left right|movement|
|backspace delete|deleting|
|ctrl w|deleting until previous word|
|ctrl a|go to start of words in line|
|ctrl e|go to end of line|
|ctrl s|save (should auto save for you tho)|
|tab|autocomplete|
|ESCAPE|go to shortcut mode|

### visual modes
- `V` `CTRL V` `v`
|key|function|
|:---:|:---:|
|up down left right hjkl w b ctrl u ctrl d| basically all the movement keys work the same|
|c| cut and go to typing mode|
|x| delete|
|d| cut|
|y| copy|
|p| delete and paste|
|e| comment / uncomment|
|<>| change indent|
|iw| highlight word|
|k| move highlighted section up
|J| move highlighted section down
|*| search in file for current highlighted text|
|#| search in project for current highlighted text|
|i<([{}])>| highlight inside of any bracket type|
|ESCAPE|go to shortcut mode|
