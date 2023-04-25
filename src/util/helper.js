/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import fs from 'fs';

function pasteFromClipboardBefore(state) {
    const systemPaste = ncp.paste().split('\n');
    if (state.clipboard !== systemPaste) {
        state.clipboard = systemPaste;
    }
    if (state.clipboard.length > 0) {
        if (state.clipboardNewLine) {
            for (let i = state.clipboard.length - 1; i >= 0; i -= 1) {
                state.data.splice(state.row, 0, state.clipboard[i]);
            }
        } else {
            const cutoff = state.data[state.row].substring(state.col);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.clipboard[0];
            let counterRow = state.row;
            for (let i = state.clipboard.length - 1; i >= 1; i -= 1) {
                state.data.splice(state.row + 1, 0, state.clipboard[i]);
                counterRow += 1;
            }
            state.data[counterRow] += cutoff;
        }
        return true;
    }
    return false;
}

function pasteFromClipboardAfter(state) {
    const systemPaste = ncp.paste().split('\n');
    if (state.clipboard !== systemPaste) {
        state.clipboard = systemPaste;
    }
    if (state.clipboard.length > 0) {
        if (state.clipboardNewLine) {
            for (let i = state.clipboard.length - 1; i >= 0; i -= 1) {
                state.data.splice(state.row + 1, 0, state.clipboard[i]);
            }
        } else {
            const cutoff = state.data[state.row].substring(state.col + 1);
            state.data[state.row] = state.data[state.row].substring(0, state.col + 1) + state.clipboard[0];
            let counterRow = state.row;
            for (let i = state.clipboard.length - 1; i >= 1; i -= 1) {
                state.data.splice(state.row + 1, 0, state.clipboard[i]);
                counterRow += 1;
            }
            state.data[counterRow] += cutoff;
        }
        return true;
    }
    return false;
}

function isAlphaNumeric(s) {
    return '1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm'.indexOf(s) > -1;
}

function isWritable(s) {
    return ' qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > -1;
}

function moveCursor(state, screen) {
    const r = state.row < state.data.length
        ? state.row
        : state.data.length - 1;
    screen.moveTo(
        (state.col < state.data[r].length
            ? state.col
            : state.data[r].length) + 5,
        (state.row < state.data.length ? state.row : state.data.length) - state.windowLine,
    );
    screen.drawCursor();
}

function isHighlighted(state, i, j) {
    if (state.mode === 'V') {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visualLine.row) || (i >= state.row && i <= state.visualLine.row)) {
            return true;
        }
    } else if (state.mode === 'v') {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i < state.row && i > state.visual.row) || (i > state.row && i < state.visual.row)) {
            return true;
        } else {
            if (i === state.row && i === state.visual.row) {
                if (state.visual.col <= state.col) {
                    if (j >= state.visual.col && j <= state.col) {
                        return true;
                    }
                } else if (state.visual.col > state.col) {
                    if (j >= state.col && j <= state.visual.col) {
                        return true;
                    }
                }
            } else if (i === state.visual.row) {
                if (j >= state.visual.col && state.row > state.visual.row) {
                    return true;
                } else if (j <= state.visual.col && state.row < state.visual.row) {
                    return true;
                }
            } else if (i === state.row) {
                if (j <= state.col && state.row > state.visual.row) {
                    return true;
                } else if (j >= state.col && state.row < state.visual.row) {
                    return true;
                }
            }
        }
    }
    return false;
}

function getColor(s, isComment, isInString) {
    if (isComment) {
        return 'green';
    }
    if (isInString) {
        return 'magenta';
    }
    if (s === '(' || s === ')') {
        return 'yellow';
    } else if (s === '"' || s === '\'' || s === '`') {
        return 'magenta';
    } else if (s === '[' || s === ']') {
        return 'green';
    } else if (s === '{' || s === '}') {
        return 'cyan';
    } else {
        return 'white';
    }
}

// sometime will have to limit number of snapshots, and make them diff based
function createSnapshot(state) {
    state.snapshots.splice(state.currentSnapshot + 1, state.snapshots.length - (state.currentSnapshot + 1));
    const oldData = [];
    for (let i = 0; i < state.data.length; i += 1) {
        oldData.push(state.data[i]);
    }
    state.snapshots.push({
        data: oldData,
        row: state.row,
        col: state.col,
        windowLine: state.windowLine
    });
    state.currentSnapshot = state.snapshots.length - 1;
    state.isSaved = false;
}

function isInString(state, row, col) {
    let currentString = '';
    let disregard = false;
    for (let i = 0; i < col; i += 1) {
        if (currentString === '' && (state.data[row].substring(i, i + 1) === '\'' || state.data[row].substring(i, i + 1) === '"' || state.data[row].substring(i, i + 1) === '`')) {
            currentString = state.data[row].substring(i, i + 1);
        } else if (currentString !== '' && (state.data[row].substring(i, i + 1) === currentString)) {
            if (!disregard) {
                currentString = '';
            }
        } else if (currentString !== '' && (state.data[row].substring(i, i + 1) === '\\')) {
            disregard = true;
        } else {
            disregard = false;
        }
    }
    return currentString !== '';
}

function commentStartsAt(state, row) {
    for(let i = 0; i < state.data[row].length; i += 1) {
        if (state.data[row].substring(i, i + 2) === '//' && !isInString(state, row, i)) {
            return i;
        }
    }
    return -1;
}

function applySnapshot(state, index) {
    const snap = state.snapshots[index];
    if (snap !== undefined) {
        state.data = [];
        for (let i = 0; i < snap.data.length; i += 1) {
            state.data.push(snap.data[i]);
        }
        state.currentSnapshot = index;
    }
}

function logCommand(newCommand, state, key) {
    if (state.allowCommandLogging && newCommand) {
        state.previousCommand = [];
    }
    if (state.allowCommandLogging) {
        state.previousCommand.push(key);
    }
}

function renderScreen(state, screen) {
    if (state.allowCommandLogging) {
        screen.fill({ char: ' ' });
        screen.moveTo(0, 0);
        for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows); i += 1) {
            if (state.data[i] !== undefined) {
                screen.put({
                    attr: {
                        color: state.isSaved || (state.currentSnapshot === 0 && state.mode !== 'i') ? 'grey' : 'white'
                    },
                    x: 0
                }, (i + 1).toString().padStart(4) + ' ');
                for (let j = 0; j < state.data[i].length; j += 1) {
                    screen.put({
                        attr: {
                            color: getColor(state.data[i].substring(j, j + 1), commentStartsAt(state, i) !== -1 && j >= commentStartsAt(state, i), isInString(state, i, j)),
                            inverse: isHighlighted(state, i, j)
                        },
                        wrap: true
                    }, state.data[i].substring(j, j + 1));
                }
                screen.put({ newLine: true }, '\n');
            }
        }
        screen.draw({ delta: false });
        moveCursor(state, screen);
    }
}

function copyToClipboard(state, textArray, newLine) {
    state.clipboard = textArray;
    state.clipboardNewLine = newLine;
    ncp.copy(state.clipboard.join('\n'));
}

function saveFile(state, term) {
    (async () => fs.writeFile(state.file, state.data.join('\n'), (err) => {
        if (err) {
            term.fullScreen(false);
            console.log(err);
            process.exit();
        }
    }))();
    state.isSaved = true;
}

function arraysEqual(a, b) {
    if (a === b) return true;
    if (a == null || b == null) return false;
    if (a.length !== b.length) return false;
    for (let i = 0; i < a.length; i += 1) {
        if (a[i] !== b[i]) return false;
    }
    return true;
}

function searchForString(state, string) {
    // TODO search backwards from top after finishing, and have another mirror function that searches backwards
    if (state.data[state.row].substring(state.col).includes(string)) {
        state.col = state.col + state.data[state.row].substring(state.col).indexOf(string);
        state.search.row = state.row;
        state.search.col = state.col + string.length;
        return true;
    }
    for (let row = state.row + 1; row < state.data.length; row += 1) {
        if (state.data[row].includes(string)) {
            state.row = row;
            state.col = state.data[row].indexOf(string);
            state.search.row = state.row;
            state.search.col = state.col + string.length;
            return true;
        }
    }
}

function centerScreen(state) {
    state.windowLine = state.row - Math.floor(process.stdout.rows / 2) >= 0 ? state.row - Math.floor(process.stdout.rows / 2) : 0
}

export {
    pasteFromClipboardBefore,
    pasteFromClipboardAfter,
    copyToClipboard,
    isAlphaNumeric,
    isWritable,
    moveCursor,
    renderScreen,
    createSnapshot,
    applySnapshot,
    logCommand,
    arraysEqual,
    saveFile,
    searchForString,
    centerScreen
};
