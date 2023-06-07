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

function shortenFilePath(filePath) {
    const parts = filePath.split('/');

    for (let i = 0; i < parts.length - 1; i += 1) {
        if (parts[i].length > 1) {
            parts[i] = parts[i][0];
        }
    }

    return parts.join('/');
}

function pasteFromClipboardBefore(state) {
    let systemPaste = ncp.paste();
    let newLine = false;
    if (systemPaste.startsWith('\n')) {
        systemPaste = systemPaste.substring(1);
        newLine = true;
    }
    systemPaste = systemPaste.split('\n');
    if (systemPaste.length > 0) {
        if (newLine) {
            for (let i = systemPaste.length - 1; i > 0; i -= 1) {
                state.data.splice(state.row, 0, systemPaste[i]);
            }
        } else {
            const cutoff = state.data[state.row].substring(state.col);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + systemPaste[0];
            let counterRow = state.row;
            for (let i = systemPaste.length - 1; i >= 1; i -= 1) {
                state.data.splice(state.row + 1, 0, systemPaste[i]);
                counterRow += 1;
            }
            state.data[counterRow] += cutoff;
        }
        return true;
    }
    return false;
}

function pasteFromClipboardAfter(state) {
    let systemPaste = ncp.paste();
    let newLine = false;
    if (systemPaste.startsWith('\n')) {
        systemPaste = systemPaste.substring(1);
        newLine = true;
    }
    systemPaste = systemPaste.split('\n');
    if (systemPaste.length > 0) {
        if (newLine) {
            for (let i = systemPaste.length - 1; i > 0; i -= 1) {
                state.data.splice(state.row + 1, 0, systemPaste[i]);
            }
        } else {
            const cutoff = state.data[state.row].substring(state.col + 1);
            state.data[state.row] = state.data[state.row].substring(0, state.col + 1) + systemPaste[0];
            let counterRow = state.row;
            for (let i = systemPaste.length - 1; i >= 1; i -= 1) {
                state.data.splice(state.row + 1, 0, systemPaste[i]);
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
    return '1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_'.indexOf(s) > -1;
}

function isWritable(s) {
    return ' qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > -1;
}

function getRowIfOverflow(state) {
    return state.row < state.data.length
        ? state.row
        : state.data.length - 1;
}

function moveCursor(state, screen, windowLineHorizontal) {
    const r = getRowIfOverflow(state);
    let row;
    let col;
    if (state.data[r] !== undefined) {
        col = state.col < state.data[r].length
            ? state.col
            : state.data[r].length;
    } else {
        col = 0;
    }
    col += 5;
    row = state.row < state.data.length ? state.row : state.data.length;
    col -= windowLineHorizontal;
    row -= state.windowLine;
    screen.moveTo(
        col,
        row + 1
    );
}

function isRowHighlighted(state, i) {
    if (state.mode === 'V') {
        if ((i <= state.row && i >= state.visualLine.row) || (i >= state.row && i <= state.visualLine.row)) {
            return true;
        }
    } else if (state.mode === 'CTRL_V') {
        if ((i <= state.row && i >= state.visualBlock.row) || (i >= state.row && i <= state.visualBlock.row)) {
            return true;
        }
    } else if (state.mode === 'v') {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else {
        return false;
    }
}

function isHighlighted(state, i, j) {
    if (state.mode === 'V') {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visualLine.row) || (i >= state.row && i <= state.visualLine.row)) {
            return true;
        }
    } else if (state.mode === 'CTRL_V') {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visualBlock.row) || (i >= state.row && i <= state.visualBlock.row)) {
            if (state.visualBlock.col <= state.col) {
                if (j >= state.visualBlock.col && j <= state.col) {
                    return true;
                }
            } else if (state.visualBlock.col > state.col) {
                if (j >= state.col && j <= state.visualBlock.col) {
                    return true;
                }
            }
        }
        return false;
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

function getCommentString(state) {
    const extension = state.file.split('.').pop().toLowerCase();
    let commentString = '';
    switch (extension) {
        case 'java':
        case 'c':
        case 'cpp':
        case 'h':
        case 'rs':
        case 'cs':
        case 'Makefile':
        case 'php':
        case 'js':
        case 'swift':
        case 'kt':
        case 'go':
        case 'scala':
        case 'ts':
        case 'jsx':
        case 'tsx':
        case 'rust':
        case 'vb':
        case 'ejs':
        case 'lua':
        case 'coffee':
        case 'groovy':
        case 'matlab':
        case 'diff':
        case 'scss':
        case 'cfg':
        case 'htm':
        case 'conf':
        case 'styl':
        case 'adoc':
        case 'cshtml':
        case 'aspx':
        case 'pug':
        case 'json':
        case 'json5':
        case 'dockerfile':
        case 'graphql':
        case 'iml':
        case 'twig':
        case 'bat':
        case 'phtml':
        case 'fish':
        case 'cake':
        case 'gd':
        case 'nim':
            commentString = '//';
            break;
        case 'sh':
        case 'bashrc':
        case 'zshrc':
        case 'yaml':
        case 'yml':
        case 'ini':
        case 'py':
        case 'rb':
        case 'gitignore':
        case 'pl':
        case 'r':
        case 'cobol':
        case 'properties':
        case 'cson':
        case 'env':
            commentString = '#';
            break;
        case 'css':
            commentString = '/*';
            break;
        case 'html':
        case 'xml':
        case 'markdown':
        case 'md':
            commentString = '<!--';
            break;
        default:
            break;
    }
    return commentString;
}

function commentStartsAt(state, row) {
    const commentString = getCommentString(state);
    let inString = false;
    let disregardNext = false;
    let stringChar;
    if (commentString === '') {
        return -1;
    }
    for (let i = 0; i < state.data[row].length; i += 1) {
        const s = state.data[row].substring(i, i + 1);
        if (s === stringChar && stringChar !== undefined && !disregardNext) {
            inString = !inString;
            stringChar = undefined;
        } else if (s === '"' || s === '\'' || s === '`') {
            if (stringChar === undefined) {
                inString = !inString;
                stringChar = s;
            }
        }
        if (state.data[row].substring(i, i + commentString.length) === commentString && !inString) {
            return i;
        }
        if (inString && s === '\\' && !disregardNext) {
            disregardNext = true;
        } else {
            disregardNext = false;
        }
    }
    return -1;
}

function logCommand(newCommand, state, key) {
    if (newCommand) {
        state.prevRow = state.row;
        state.prevCol = state.col;
    }
    if (state.allowCommandLogging) {
        if (newCommand) {
            state.previousCommand = [];
        }
        state.previousCommand.push(key);
    }
}

function getColorRow(replacing, replaceQuery, row, commentIndex, searching, searchQuery, isCurrentRow, col) {
    const output = [];
    let inString = false;
    let stringChar;
    let color = 'white';
    let disregardNext = false;
    let stop = false;
    for (let i = 0; i < row.length; i += 1) {
        if (row.length > 180) {
            output.push('red');
        } else {
            const s = row.substring(i, i + 1);
            if (inString) {
                color = 'magenta';
            }
            if (s === stringChar && stringChar !== undefined && !disregardNext) {
                inString = !inString;
                stringChar = undefined;
            } else if (s === '"' || s === '\'' || s === '`') {
                color = 'magenta';
                if (stringChar === undefined) {
                    inString = !inString;
                    stringChar = s;
                }
            } else if (!inString && (s === '(' || s === ')')) {
                color = 'yellow';
            } else if (!inString && (s === '[' || s === ']')) {
                color = 'green';
            } else if (!inString && (s === '{' || s === '}')) {
                color = 'cyan';
            } else if ((replacing || searching) && searchQuery.length !== 0 && searchQuery === row.substring(i, i + searchQuery.length)) {
                if (replacing) {
                    for (let j = 0; j < replaceQuery.length; j += 1) {
                        output.push('search');
                    }
                } else if (searching) {
                    for (let j = 0; j < searchQuery.length; j += 1) {
                        if (isCurrentRow && i <= col && i + searchQuery.length >= col) {
                            output.push('searchCurrent');
                        } else {
                            output.push('search');
                        }
                    }
                }
                i += searchQuery.length - 1;
                stop = true;
            }
            if (i >= commentIndex && commentIndex !== -1) {
                color = 'green';
            }
            if (!stop) {
                output.push(color);
            }
            if (inString && s === '\\' && !disregardNext) {
                disregardNext = true;
            } else {
                disregardNext = false;
            }
            color = 'white';
            stop = false;
        }
    }
    return output;
}

function copyToClipboard(state, textArray, clipboardVisualBlock) {
    state.clipboardVisualBlock = clipboardVisualBlock;
    ncp.copy(textArray.join('\n'));
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
    if (string.length === 0) {
        return false;
    }
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
    return state.row >= state.windowLine && state.row <= state.windowLine + process.stdout.rows - 1;
}

function centerScreen(state) {
    state.windowLine = state.row - Math.floor(process.stdout.rows / 2) >= 0 ? state.row - Math.floor(process.stdout.rows / 2) : 0;
}

function getWindowLineHorizontal(state) {
    let col;
    if (state.data[state.row] !== undefined) {
        col = state.col < state.data[state.row].length
            ? state.col
            : state.data[state.row].length;
    } else {
        col = 0;
    }
    if (col - state.windowLineHorizontal > process.stdout.columns - 6) {
        state.windowLineHorizontal = (col - process.stdout.columns + 6) + 10;
    } else if (col - state.windowLineHorizontal < 0) {
        state.windowLineHorizontal = col - 10 > 0 ? col - 10 : 0;
    }
}

function isMergeConflictStart(s) {
    return s.startsWith('<<<<<<<');
}

function isMergeConflictMiddle(s) {
    return s.startsWith('=======');
}

function isMergeConflictEnd(s) {
    return s.startsWith('>>>>>>>');
}

function renderStatusBar(state, screen) {
    for (let i = 0; i < state.harpoonIndexes.length; i += 1) {
        screen.put({ attr: { color: i === state.harpoonIndex ? 'yellow' : 'grey' } }, shortenFilePath(state.files[state.harpoonIndexes[i]]) + ' ');
    }
    let index = 6 + state.file.length + ((state.row + 1).toString().length) + ((state.row + 1).toString().length) + state.searchQuery.length;
    screen.put({ attr: { color: 'grey' }, x: process.stdout.columns - index - 21 }, state.commandHistory.slice(-20));
    screen.put({ attr: { color: 'green' }, x: process.stdout.columns - index }, '/' + state.searchQuery + ' ');
    screen.put({ attr: { color: 'white' } }, '"' + state.file + ':' + (state.row + 1) + ':' + (state.col + 1) + '"');
}

function renderFileFinder(state, screen, mode) {
    screen.put({
        attr: {
            color: mode === 'g' ? 'green' : 'red',
        },
        x: 0,
        wrap: false
    }, '> ');
    screen.put({
        attr: {
            color: 'white',
        },
        wrap: false
    }, state.fileFinderQuery);
    let index = state.fileFinderIndex - Math.floor(process.stdout.rows / 2) > 0 ? state.fileFinderIndex - Math.floor(process.stdout.rows / 2) : 0;
    for (let i = index; i < state.fileFindingOutput.length; i += 1) {
        screen.put({ newLine: true }, '\n');
        screen.put({
            attr: {
                color: state.fileFinderIndex === i ? 'green' : 'white',
            },
            x: 0,
            wrap: false
        }, state.fileFindingOutput[i]);
    }
    screen.moveTo(
        state.fileFinderQuery.length + 2,
        1
    );
    screen.draw({ delta: true });
    screen.drawCursor();
}

function renderSingleLine(state, screen, i, mergeSection) {
    screen.put({
        attr: {
            color: state.recording ? 'red' : 'grey'
        },
        x: 0
    }, (i + 1).toString().padStart(4) + ' ');
    if (isMergeConflictStart(state.data[i])) {
        mergeSection = 1;
    } else if (isMergeConflictMiddle(state.data[i])) {
        mergeSection = 2;
    } else if (isMergeConflictEnd(state.data[i])) {
        mergeSection = 0;
    }
    const commentIndex = commentStartsAt(state, i);
    const colorRow = getColorRow(
        state.replacing,
        state.replaceQuery,
        state.data[i],
        commentIndex,
        state.searching,
        state.searchQuery,
        state.row === i,
        state.col
    );
    let displayRow = state.data[i];
    if (state.replacing) {
        displayRow = displayRow.replaceAll(state.searchQuery, state.replaceQuery);
    }
    for (let j = state.windowLineHorizontal; j < displayRow.length; j += 1) {
        let color = colorRow[j];
        let bgColor;
        if (mergeSection === 1 && !isMergeConflictStart(displayRow)) {
            color = 'red';
        } else if (mergeSection === 2 && !isMergeConflictMiddle(displayRow)) {
            color = 'green';
        } else if (isMergeConflictEnd(displayRow) || isMergeConflictMiddle(displayRow) || isMergeConflictStart(displayRow)) {
            color = 'blue';
        }
        if (colorRow[j] === 'search') {
            if (state.replacing) {
                color = 'white';
                bgColor = 'blue';
            } else if (state.searching) {
                color = 'black';
                bgColor = 'green';
            }
        } else if (colorRow[j] === 'searchCurrent') {
            color = 'black';
            bgColor = 'magenta';
        }
        screen.put({
            attr: {
                color,
                bgColor,
                inverse: isHighlighted(state, i, j)
            },
            wrap: false
        }, displayRow.substring(j, j + 1));
    }
    if (state.data[i] === '' && isRowHighlighted(state, i)) {
        screen.put({
            attr: {
                inverse: true
            },
            wrap: false
        }, ' ');
    }
    screen.put({ newLine: true }, '\n');
}

function renderWindowLines(state, screen, noCenterScreen, fullRefresh) {
    if (!noCenterScreen && !isOnScreen(state)) {
        centerScreen(state);
    }
    getWindowLineHorizontal(state);
    const mergeSection = 0;
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows) - 1; i += 1) {
        if (state.data[i] !== undefined) {
            renderSingleLine(state, screen, i, mergeSection);
        }
    }
    moveCursor(state, screen, state.windowLineHorizontal);
    screen.draw({ delta: !fullRefresh });
    screen.drawCursor();
}

function setHarpoonIndex(state) {
    if (state.harpoonIndexes.includes(state.fileIndex)) {
        state.harpoonIndex = state.harpoonIndexes.indexOf(state.fileIndex);
    }
}

function renderScreen(state, screen, noCenterScreen, fullRefresh) {
    if (state.data.length === 0) {
        state.data = [''];
    }
    screen.fill({ char: ' ' });
    screen.moveTo(0, 0);
    setHarpoonIndex(state);
    renderStatusBar(state, screen);
    screen.put({ newLine: true }, '\n');
    if (state.mode === 'g' || state.mode === 'f') {
        renderFileFinder(state, screen, state.mode);
    } else if (state.allowCommandLogging) {
        renderWindowLines(state, screen, noCenterScreen, fullRefresh);
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

function changeFile(state) {
    state.mode = 'n';
    state.row = 0;
    state.col = 0;
    state.windowLine = 0;
    state.windowLineHorizontal = 0;
    state.snapshots = [];
    state.currentSnapshot = 0;
    state.savePoint = 0;
    state.recording = false;
    state.mark = -1;
    state.data = getData(state.file);
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
    getRowIfOverflow,
    changeFile,
    getData
};
