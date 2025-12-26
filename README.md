# setup

MACOS AND LINUX ONLY

install g++ and make (also xclip if on xorg)
```
sudo make install
```

# keys

I implemented the vim keys that I use, here are the extra ones I added on top

### normal mode

|key|description|
| space | bookmark current file |
| ctrl h | move bookmark left one space |
| ctrl l | move bookmark right one space |
| ctrl x | delete current bookmark |
| ctrl w | switch to left bookmark |
| ctrl e | switch to right bookmark |
| X | clear all bookmarks |
| e | toggle comment on current line |
| g + e | find next commented block of code and uncomment it |
| ctrl g | open grep search |
| ctrl p | open file search |
| ctrl t | open file explorer |
| M | mark current position |
| ' | jump to marked position |
| J | join next line with current line |
| K | join next line with current line with a space in between |
| H | enter command mode with :e currentFilePath |
| ctrl a | increment current number under cursor |
| ctrl s | decrement current number under cursor |
| ctrl y | toggle show git blame for current file (ctrl y again copies git hash to clipboard) |
| ctrl f | change to replace mode |
| m | toggle logging for current variable |
| Q | clear all logging variables in current file |
| \\ | refresh current file (currently don't have a prompt yet for external changes) |
| g + \/ | enter search mode without clearing current search |
| g + r | navigate to definition of current word (buggy, need to get lsp) |
| g + t | trim all trailing whitespace in current file |
| g + y | copy current file path to clipboard |
| ctrl z | go backwards through everywhere the cursor has been in current file |
| ctrl q | go forwards through everywhere the cursor has been in current file |
| ctrl o | go backward in recent file stack |
| ctrl i | go forward in recent file stack |
| \# | grep search current word |
| * | search current word |

## insert mode

| ctrl t | insert tab |
| ctrl v + any character | insert ctrl version of character |
| TAB | autocomplete |

## command mode

Mode for settings or saving or quitting.

|key|description|
|:---:|:---:|
| :gs/search/replace/ | global search replace in current project using git files |
| ctrl(r) | copy search text to replace text |

## visual mode

|key|description|
|:---:|:---:|
| e | comment highlighted text |
| g + r | jump to definition of highlighted text |
| g + f | treat highlighted text as filepath and try to jump to file |
| ctrl(f) | search/replace inside of visual |
| : | put selected text as global search replace |
| * | search highlighted text |
| \# | grep search highlighted text |

## file explorer mode

|key|description|
|:---:|:---:|
| escape | go back to normal mode |
| k or ctrl p | up |
| j or ctrl n | down |
| enter | open file, or toggle folder open/closed |
| ctrl u | up half screen |
| ctrl d | down half screen |
| p | paste |
| y | copy |
| n | create new file |
| N | create new folder |
| X | delete file |
| R | rename file |
| r | refresh file explorer |
| ctrl h | make file explorer smaller |
| ctrl l | make file explorer bigger |
| ctrl t | go back to normal mode while keeping file explorer open |
| f | open file in background |
| - | collapse all |
| ctrl g | grep at filepath |

# config file (~/.editorconfig)

.editorconfig (default settings)
```
wordwrap = true
autosave = true
insert_final_newline = false
indent_size = 4
indent_style = space
```

autosave = false kinda doesn't work well with multiple files since I reload the full file each time
wordwrap = false also doesn't work well with tabs

