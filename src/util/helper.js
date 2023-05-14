/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import fs from 'fs';

function getData(filepath) {
    try {
        return (fs.readFileSync(filepath, 'utf-8')).split('\n');
    } catch (e) {
        console.log(filepath + ' not found\nusage: edit [file]');
        process.exit();
    }
}

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

function isNumeric(s) {
    return '1234567890'.indexOf(s) > -1;
}

function isAlphaNumeric(s) {
    return '1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm'.indexOf(s) > -1;
}

function isWritable(s) {
    return ' qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > -1;
}

function moveCursor(state, screen, windowLineHorizontal) {
    const r = state.row < state.data.length
        ? state.row
        : state.data.length - 1;
    screen.moveTo(
        state.data[r]
            ? (state.col < state.data[r].length
                ? state.col
                : state.data[r].length) + 5 - windowLineHorizontal
            : 0 + 5 - windowLineHorizontal,
        (state.row < state.data.length ? state.row : state.data.length) - state.windowLine,
    );
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

function isInStringRow(row, col) {
    let currentString = '';
    let disregard = false;
    for (let i = 0; i < col; i += 1) {
        if (currentString === '' && (row.substring(i, i + 1) === '\'' || row.substring(i, i + 1) === '"' || row.substring(i, i + 1) === '`')) {
            currentString = row.substring(i, i + 1);
        } else if (currentString !== '' && (row.substring(i, i + 1) === currentString)) {
            if (!disregard) {
                currentString = '';
            }
        } else if (currentString !== '' && (row.substring(i, i + 1) === '\\')) {
            disregard = true;
        } else {
            disregard = false;
        }
    }
    return currentString !== '';
}

function isInString(state, row, col) {
    let currentString = '';
    let disregard = false;
    for (let i = 0; i < col; i += 1) {
        const currentChar = state.data[row].substring(i, i + 1);
        if (currentString === '' && (currentChar === '\'' || currentChar === '"' || currentChar === '`')) {
            currentString = currentChar;
        } else if (currentString !== '' && (currentChar === currentString)) {
            if (!disregard) {
                currentString = '';
            }
        } else if (currentString !== '' && (currentChar === '\\')) {
            disregard = true;
        } else {
            disregard = false;
        }
    }
    return currentString !== '';
}

function commentStartsAt(state, row) {
    for (let i = 0; i < state.data[row].length; i += 1) {
        if (state.file.endsWith('.js')
            || state.file.endsWith('.jsx')
            || state.file.endsWith('.tsx')
            || state.file.endsWith('.ts')
            || state.file.endsWith('.java')
            || state.file.endsWith('.c')
            || state.file.endsWith('.cpp')
            || state.file.endsWith('.rs')) {
            if (state.data[row].substring(i, i + 2) === '//' && !isInString(state, row, i)) {
                return i;
            }
        } else if (state.file.endsWith('.sh')
            || state.file.endsWith('.py')
            || state.file.endsWith('.bashrc')
            || state.file.endsWith('.zshrc')
            || state.file.endsWith('.env')) {
            if (state.data[row].substring(i, i + 1) === '#' && !isInString(state, row, i)) {
                return i;
            }
        }
    }
    return -1;
}

function logCommand(newCommand, state, key) {
    if (state.allowCommandLogging) {
        if (newCommand) {
            state.previousCommand = [];
            state.prevRow = state.row;
            state.prevCol = state.col;
        }
        state.previousCommand.push(key);
    }
}

function getColorRow(row, commentIndex, searching, searchQuery) {
    const output = [];
    for (let i = 0; i < row.length; i += 1) {
        const s = row.substring(i, i + 1);
        if (searching && searchQuery === row.substring(i, i + searchQuery.length)) {
            for (let j = 0; j < searchQuery.length; j += 1) {
                output.push('search');
            }
            i += searchQuery.length - 1;
        } else if (i >= commentIndex && commentIndex !== -1) {
            output.push('green');
        } else if (isInStringRow(row, i)) {
            output.push('magenta');
        } else if (s === '(' || s === ')') {
            output.push('yellow');
        } else if (s === '"' || s === '\'' || s === '`') {
            output.push('magenta');
        } else if (s === '[' || s === ']') {
            output.push('green');
        } else if (s === '{' || s === '}') {
            output.push('cyan');
        } else {
            output.push('white');
        }
    }
    return output;
}

function copyToClipboard(state, textArray, newLine) {
    state.clipboard = textArray;
    state.clipboardNewLine = newLine;
    ncp.copy(state.clipboard.join('\n'));
}

async function saveFile(state) {
    if (state.currentSnapshot !== state.savePoint) {
        await (async () => fs.writeFile(state.file, state.data.join('\n'), (err) => {
            if (err) {
                console.log(err);
                process.exit();
            }
        }))();
        state.savePoint = state.currentSnapshot;
    }
}

function applySnapshot(state, index, backwards) {
    const snap = state.snapshots[index];
    if (snap !== undefined) {
        state.data = [];
        for (let i = 0; i < snap.data.length; i += 1) {
            state.data.push(snap.data[i]);
        }
        state.currentSnapshot = index;
        const pos = backwards ? index + 1 : index;
        if (state.snapshots[pos]) {
            state.row = state.snapshots[pos].row;
            state.col = state.snapshots[pos].col;
        }
    }
}

function createSnapshot(state) {
    state.snapshots.splice(state.currentSnapshot + 1, state.snapshots.length - (state.currentSnapshot + 1));
    const oldData = [];
    for (let i = 0; i < state.data.length; i += 1) {
        oldData.push(state.data[i]);
    }
    state.snapshots.push({
        data: oldData,
        row: state.prevRow,
        col: state.prevCol,
        windowLine: state.windowLine
    });
    state.currentSnapshot = state.snapshots.length - 1;
    saveFile(state);
}

function searchBackForString(state, string) {
    if (state.data[state.row].substring(0, state.col).includes(string)) {
        state.col = state.data[state.row].substring(0, state.col).lastIndexOf(string);
        return true;
    }
    for (let row = state.row - 1; row >= 0; row -= 1) {
        if (state.data[row].includes(string)) {
            state.row = row;
            state.col = state.data[row].lastIndexOf(string);
            return true;
        }
    }
    for (let row = state.data.length - 1; row > state.row; row -= 1) {
        if (state.data[row].includes(string)) {
            state.row = row;
            state.col = state.data[row].lastIndexOf(string);
            return true;
        }
    }
    if (state.data[state.row].includes(string)) {
        state.col = state.data[state.row].lastIndexOf(string);
        return true;
    }
    return false;
}

function searchForStringNoWrap(state, string) {
    if (state.data[state.row].substring(state.col).includes(string)) {
        state.col += state.data[state.row].substring(state.col).indexOf(string);
        return true;
    }
    for (let row = state.row + 1; row < state.data.length; row += 1) {
        if (state.data[row].includes(string)) {
            state.row = row;
            state.col = state.data[row].indexOf(string);
            return true;
        }
    }
    return false;
}

function searchForString(state, string) {
    if (state.data[state.row].substring(state.col).includes(string)) {
        state.col += state.data[state.row].substring(state.col).indexOf(string);
        return true;
    }
    for (let row = state.row + 1; row < state.data.length; row += 1) {
        if (state.data[row].includes(string)) {
            state.row = row;
            state.col = state.data[row].indexOf(string);
            return true;
        }
    }
    for (let row = 0; row < state.row; row += 1) {
        if (state.data[row].includes(string)) {
            state.row = row;
            state.col = state.data[row].indexOf(string);
            return true;
        }
    }
    if (state.data[state.row].includes(string)) {
        state.col = state.data[state.row].indexOf(string);
        return true;
    }
    return false;
}

function isOnScreen(state) {
    return state.row >= state.windowLine && state.row < state.windowLine + process.stdout.rows;
}

function centerScreen(state) {
    state.windowLine = state.row - Math.floor(process.stdout.rows / 2) >= 0 ? state.row - Math.floor(process.stdout.rows / 2) : 0;
}

function getWindowLineHorizontal(state) {
    if (state.col - state.windowLineHorizontal > process.stdout.columns - 6) {
        state.windowLineHorizontal = state.col - process.stdout.columns + 6;
    } else if (state.col - state.windowLineHorizontal < 0) {
        state.windowLineHorizontal = state.col;
    }
}

async function renderScreen(state, screen, noCenterScreen) {
    await (() => {
        if (state.data.length === 0) {
            state.data = [''];
        }
    })();
    if (state.allowCommandLogging) {
        if (!noCenterScreen && !isOnScreen(state)) {
            centerScreen(state);
        }
        getWindowLineHorizontal(state);
        screen.fill({ char: ' ' });
        screen.moveTo(0, 0);
        for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows); i += 1) {
            if (state.data[i] !== undefined) {
                screen.put({
                    attr: {
                        color: state.recording ? 'red' : 'grey'
                    },
                    x: 0
                }, (i + 1).toString().padStart(4) + ' ');
                const commentIndex = commentStartsAt(state, i);
                const colorRow = getColorRow(state.data[i], commentIndex, state.searching, state.searchQuery);
                for (let j = state.windowLineHorizontal; j < state.data[i].length; j += 1) {
                    screen.put({
                        attr: {
                            color: colorRow[j] === 'search' ? 'black' : colorRow[j],
                            bgColor: colorRow[j] === 'search' ? 'green' : undefined,
                            inverse: isHighlighted(state, i, j)
                        },
                        wrap: false
                    }, state.data[i].substring(j, j + 1));
                }
                screen.put({ newLine: true }, '\n');
            }
        }
        moveCursor(state, screen, state.windowLineHorizontal);
        await screen.draw({ delta: true });
        await screen.drawCursor();
    }
}

function trimTrailingWhitespace(state) {
    for (let i = 0; i < state.data.length; i += 1) {
        state.data[i] = state.data[i].trimEnd();
    }
}

function refreshFile(state) {
    const newData = getData(state.file);
    state.data = [];
    for (let i = 0; i < newData.length; i += 1) {
        state.data.push(newData[i]);
    }
    createSnapshot(state);
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
    saveFile,
    searchForStringNoWrap,
    searchForString,
    searchBackForString,
    isOnScreen,
    isNumeric,
    isHighlighted,
    trimTrailingWhitespace,
    centerScreen,
    refreshFile,
    getData
};
