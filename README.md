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

### misc keys

|key|description|
|:---:|:---:|
| escape | cancel multi-key shortcut |
| r + any character | replace current character |
| \/ | enter search mode |

# FAQ

how to get rid of delay when using escape key?

put this in ~/.bashrc

```
export ESCDELAY=0
```
