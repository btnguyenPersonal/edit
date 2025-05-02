# edit
A term based ide with
- only keyboard
- a million shortcuts
- vim based

# setup

requires g++|13
requires xclip (on linux)

```
sudo make install
```

# documentation

## shortcut mode

shortcut mode is the starting mode. every key is a shortcut.

### movement keys

|key|description|
|:---:|:---:|
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
| d + t + any character | delete everything until one before character |
| d + f + any character | delete everything until character |
| D | delete until end of line |
| d + w | delete forward to next word |
| d + b | delete forward to next word |

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
| escape | cancel multi-key shortcut |
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
| ctrl y | toggle show git blame for current file |
| ctrl f | change to replace mode |
| s | delete current character under cursor and change to typing mode |
| x | delete current character under cursor and copy to clipboard |
| r + any character | replace current character |
| \/ | enter search mode |
| m | toggle logging for current variable |
| Q | clear all logging variables in current file |
| % | jump to matching brace works with (){}[] |
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
| ctrl + z | go backwards through everywhere the cursor has been in current file |
| ctrl + q | go forwards through everywhere the cursor has been in current file |
| ~ | switch upper/lower case of current character |
| : | enter commandline mode |
| < | de-indent current line |
| > | indent current line |
| ctrl o | go backward in recent file stack |
| ctrl i | go forward in recent file stack |
| \# | grep search current word |
| * | search current word |

# FAQ

how to get rid of delay when using escape key?

put this in ~/.bashrc

```
export ESCDELAY=0
```
