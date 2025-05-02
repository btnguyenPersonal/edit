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

### delete keys

|key|description|
|:---:|:---:|
| d + d | delete current line |

### misc keys

|key|description|
|:---:|:---:|
| escape | cancel multi-key shortcut |
| ctrl g | open grep search |
| ctrl p | open file search |
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
| u | undo |
| r | redo |
| r + any character | replace current character |
| \/ | enter search mode |
| g + \/ | enter search mode without clearing current search |
| g + r | navigate to definition of current word (buggy) |
| g + e | find next commented block of code and uncomment it |
| g + t | trim all trailing whitespace in current file |
| g + y | copy current file path to clipboard |
| g + g | jump to top of file |
| g + k | go up one line visually (treat wrapped lines as code lines) |
| g + j | go down one line visually (treat wrapped lines as code lines) |
| space | bookmark current file |
| 1 2 3 4 5 6 7 8 9 | jump to number bookmark |
| g + 1 2 3 4 5 6 7 8 9 | reassign to number bookmark |
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
