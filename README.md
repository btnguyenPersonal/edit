# setup

requires g++-13
requires xclip (on linux)

```
sudo make install
```

# documentation

## shortcut mode

shortcut mode is the starting mode. every key is a shortcut.
i spend 80% of my time in this mode.

### movement keys

|key|description|
|:---:|:---:|
| escape | cancel multi-key shortcut, or if not in shortcut mode go back to shortcut mode |
| h or left | move left |
| l or right | move right |
| j or down | move down |
| k or up | move up |
| ctrl u | move up half screen |
| ctrl d | move down half screen |
| w | move forward one word |
| b | move backward one word |
| f + any character | move onto character |
| t + any character | move to one before character |
| { | go to next empty line |
| } | go to previous empty line |
| [ | go to next line with the same indent level |
| ] | go to previous line with the same indent level |
| g + g | jump to top of file |
| G | jump to bottom of file |
| ^ | jump to first word on current line |
| 0 | jump to start of line |
| $ | jump to end of line |
| z | center screen |
| % | jump to matching brace works with (){}[] |

### delete keys

(all of these keys copy the deleted text to clipboard)

(all of the d + i commands also work with d + a to delete everything **including** the surrounding characters)

|key|description|
|:---:|:---:|
| d + d | delete current line |
| d + j | delete current line and next line |
| d + k | delete current line and previous line |
| d + i + w | delete current word |
| d + i + ' | delete everything inside of '' quotes |
| d + i + " | delete everything inside of "" quotes |
| d + i + \` | delete everything inside of \`\` quotes |
| d + i + b or d + i + ( | delete everything inside of () brackets |
| d + i + B or d + i + { | delete everything inside of {} brackets |
| d + i + d or d + i + [ | delete everything inside of [] brackets |
| d + i + t | delete everything inside of <> brackets |
| d + i + T | delete everything inside of <></> brackets |
| d + i + W | delete current word (includes special characters) |
| d + i + p | delete current paragraph (surrounded by empty lines) |
| d + t + any character | delete everything until one before character |
| d + f + any character | delete everything until character |
| D | delete until end of line |
| d + w | delete forward to next word |
| d + b | delete forward to next word |

### copy keys

(all of the y + i commands also work with y + a to copy everything **including** the surrounding characters)

|key|description|
|:---:|:---:|
| y + y | copy current line |
| y + j | copy current line and next line |
| y + k | copy current line and previous line |
| y + i + w | copy current word |
| y + i + ' | copy everything inside of '' quotes |
| y + i + " | copy everything inside of "" quotes |
| y + i + \` | copy everything inside of \`\` quotes |
| y + i + b or d + i + ( | copy everything inside of () brackets |
| y + i + B or d + i + { | copy everything inside of {} brackets |
| y + i + d or d + i + [ | copy everything inside of [] brackets |
| y + i + t | copy everything inside of <> brackets |
| y + i + T | copy everything inside of <></> brackets |
| y + i + W | copy current word (includes special characters) |
| y + i + p | copy current paragraph (surrounded by empty lines) |
| y + t + any character | copy everything until one before character |
| y + f + any character | copy everything until character |
| Y | copy until end of line |
| y + w | copy forward to next word |
| y + b | copy forward to next word |

### change keys

(all of the c + i commands also work with c + a to copy everything **including** the surrounding characters)

|key|description|
|:---:|:---:|
| c + c | delete current line and change to typing mode |
| c + j | delete current line and next line and change to typing mode |
| c + k | delete current line and previous line and change to typing mode |
| c + i + w | delete current word and change to typing mode |
| c + i + ' | delete everything inside of '' quotes and change to typing mode |
| c + i + " | delete everything inside of "" quotes and change to typing mode |
| c + i + \` | delete everything inside of \`\` quotes and change to typing mode |
| c + i + b or d + i + ( | delete everything inside of () brackets and change to typing mode |
| c + i + B or d + i + { | delete everything inside of {} brackets and change to typing mode |
| c + i + d or d + i + [ | delete everything inside of [] brackets and change to typing mode |
| c + i + t | delete everything inside of <> brackets and change to typing mode |
| c + i + T | delete everything inside of <></> brackets and change to typing mode |
| c + i + W | delete current word (includes special characters) and change to typing mode |
| c + i + p | delete current paragraph (surrounded by empty lines) and change to typing mode |
| c + t + any character | delete everything until one before character and change to typing mode |
| c + f + any character | delete everything until character and change to typing mode |
| C | delete until end of line and change to typing mode |
| c + w | delete forward to next word and change to typing mode |
| c + b | delete forward to next word and change to typing mode |

### file bookmark keys

|key|description|
|:---:|:---:|
| space | bookmark current file |
| ctrl h | move bookmark left one space |
| ctrl l | move bookmark right one space |
| ctrl x | delete current bookmark |
| ctrl w | switch to left bookmark |
| ctrl e | switch to right bookmark |
| X | clear all bookmarks |
| 1 2 3 4 5 6 7 8 9 | jump to number bookmark |
| g + 1 2 3 4 5 6 7 8 9 | reassign to number bookmark |

### misc keys

|key|description|
|:---:|:---:|
| u | undo |
| ctrl r | redo |
| ctrl g | open grep search |
| ctrl p | open file search |
| ctrl t | open file explorer |
| ctrl v | enter visual block mode |
| v | enter visual mode |
| V | enter visual line mode |
| i | enter typing mode |
| a | enter typing mode and move forward one character |
| o | enter typing mode and insert a newline under current line |
| O | enter typing mode and insert a newline above current line |
| I | enter typing mode and go to start of line |
| A | enter typing mode and go to end of line |
| M | mark current position |
| ' | jump to marked position |
| n | jump to next search result |
| N | jump to previous search result |
| . | repeat previous shortcut |
| q | toggle recording macro |
| , | repeat previous macro |
| J | join next line with current line |
| K | join next line with current line with a space in between |
| p | paste from clipboard after cursor |
| P | paste from clipboard before cursor |
| ctrl a | increment current number under cursor |
| ctrl s | decrement current number under cursor |
| ctrl y | toggle show git blame for current file (ctrl y again copies git hash to clipboard) |
| ctrl f | change to replace mode |
| s | delete current character under cursor and change to typing mode |
| x | delete current character under cursor and copy to clipboard |
| r + any character | replace current character |
| \/ | enter search mode |
| m | toggle logging for current variable |
| Q | clear all logging variables in current file |
| \\ | refresh current file (TODO pull in external changes automatically) |
| = | indent current line |
| e | toggle comment on current line |
| g + \/ | enter search mode without clearing current search |
| g + r | navigate to definition of current word (buggy) |
| g + e | find next commented block of code and uncomment it |
| g + t | trim all trailing whitespace in current file |
| g + y | copy current file path to clipboard |
| g + k | go up one line visually (treat wrapped lines as code lines) |
| g + j | go down one line visually (treat wrapped lines as code lines) |
| ctrl z | go backwards through everywhere the cursor has been in current file |
| ctrl q | go forwards through everywhere the cursor has been in current file |
| ~ | switch upper/lower case of current character |
| : | enter commandline mode |
| < | de-indent current line |
| > | indent current line |
| ctrl o | go backward in recent file stack |
| ctrl i | go forward in recent file stack |
| \# | grep search current word |
| * | search current word |

## typing mode

typing mode is the mode that acts like a normal keyboard.

|key|description|
|:---:|:---:|
| escape | go back to shortcuts mode |
| any character | insert character |
| backspace | delete one character backwards |
| ctrl w | delete one word backwards |
| ctrl t | insert tab |
| ctrl v + any character | insert ctrl version of character |
| TAB | autocomplete |
| left | move left |
| right | move right |
| up | move up |
| down | move down |

## command mode

mode for settings or saving or quitting.

|key|description|
|:---:|:---:|
| escape | go back to shortcuts mode |
| :q | quit |
| :wq or :x | save and quit |
| :q! | force quit |
| :w | save |
| :a | toggle autosave |
| :wordwrap | toggle wordwrap |
| :ts=any number | set your indent_size to number |

## visual mode, visual line mode, visual block mode

mode for highlighting text.
visual mode is normal editor highlighting.
visual line mode highlights full lines.
visual block mode highlightes in blocks.

(all movement keys work in this mode)

|key|description|
|:---:|:---:|
| x | delete highlighted text and don't copy to clipboard |
| d | delete highlighted text |
| p | replace highlighted text with clipboard |
| c | delete highlighted text and move to typing mode |
| e | comment highlighted text |
| o | swap cursor with starting position |
| g + r | jump to definition of highlighted text |
| g + f | treat highlighted text as filepath and try to jump to file |
| g + U | change highlighted text to uppercase |
| g + u | change highlighted text to lower |
| r + any character | replace every character in highlighted text with character |
| * | search highlighted text |
| \# | grep search highlighted text |
| = | fix indent of highlighted text |
| K | move highlighted text up one line |
| J | move highlighted text down one line |
| < | deindent highlighted text |
| > | indent highlighted text |
| I | multi-cursor mode at start of block (visual block mode only) |
| A | multi-cursor mode at end of block (visual block mode only) |

## file explorer mode

(this kinda sucks i need to fix up the rough edges)

|key|description|
|:---:|:---:|
| escape | go back to shortcuts mode |
| k or ctrl p | up |
| j or ctrl n | down |
| enter | open file, or toggle folder open/closed |
| ctrl u | up half screen |
| ctrl d | down half screen |
| p | paste file |
| y | copy file (TODO add copy folders) |
| n | create new file |
| N | create new folder |
| X | delete file |
| R | rename file |
| r | refresh file explorer |
| ctrl h | make file explorer smaller |
| ctrl l | make file explorer bigger |
| ctrl t | go back to shortcuts mode while keeping file explorer open |
| f | open file in background |
| - | collapse all |
| ctrl g | grep at filepath |

# config file (~/.editorconfig)

.editorconfig
```
# turn on word wrap
wordwrap
# set indent_size to 4
indent_size = 4
```

# faq

how to get rid of delay when using escape key?

put this in ~/.bashrc

```
export ESCDELAY=0
```
