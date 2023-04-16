/* eslint-disable import/no-cycle */
import fs from 'fs';
import { handleKeys } from '../keybinds/normalKeys.js';
import { handleVimKeys } from '../keybinds/vimKeys.js';
import { handleVisualKeys } from '../keybinds/visualKeys.js';
import { handleVisualLineKeys } from '../keybinds/visualLineKeys.js';

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
            ? state.col + 1
            : state.data[r].length + 1) + 4,
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

function getColor(s) {
    if (s === '(' || s === ')') {
        return 'yellow';
    } else if (s === '"' || s === '\'') {
        return 'red';
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

function applySnapshot(state, index) {
    const snap = state.snapshots[index];
    if (snap !== undefined) {
        state.data = [];
        for (let i = 0; i < snap.data.length; i += 1) {
            state.data.push(snap.data[i]);
        }
        state.row = snap.row;
        state.col = snap.col;
        state.windowLine = snap.windowLine;
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

function sendKeys(keys, state, screen, term) {
    for (let i = 0; i < keys.length; i += 1) {
        if (state.vim && state.mode === 'v') {
            handleVisualKeys(keys[i], state, screen, term);
        } else if (state.vim && state.mode === 'V') {
            handleVisualLineKeys(keys[i], state, screen, term);
        } else if (state.vim && state.mode === 'n' || state.mode === 'r') {
            handleVimKeys(keys[i], state, screen, term);
        } else {
            handleKeys(keys[i], state, screen);
        }
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
                            color: getColor(state.data[i].substring(j, j + 1)),
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

export {
    isAlphaNumeric,
    isWritable,
    moveCursor,
    sendKeys,
    renderScreen,
    createSnapshot,
    applySnapshot,
    logCommand,
    arraysEqual,
    saveFile
};
