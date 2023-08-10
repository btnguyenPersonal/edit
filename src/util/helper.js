/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import { execSync } from 'child_process';
import fs from 'fs';
import { isEmptyRow, getIndentLevelFrom } from './movement.js';
import {
    TYPING,
    SHORTCUTS,
    COMMAND,
    GREP,
    FILEFINDER,
    VISUAL,
    VISUALLINE,
    VISUALBLOCK,
    HISTORY,
    SEARCH,
    MULTICURSOR
} from './modes.js';

function getCurrentWord(sentence, cursorPos) {
    let start = cursorPos;
    if (sentence[start] !== undefined && /[A-Za-z0-9_]/.test(sentence[start])) {
        return '';
    }

    while (start > 0 && /[A-Za-z0-9_]/.test(sentence[start - 1])) {
        start -= 1;
    }

    return sentence.substring(start, cursorPos);
}

function autocomplete(inputString, array) {
    if (inputString === '') {
        return '';
    }

    const counts = new Map();

    for (let i = 0; i < array.length; i += 1) {
        const substrings = array[i].match(/\b\w+\b/g) || [];

        for (let j = 0; j < substrings.length; j += 1) {
            const substring = substrings[j];
            if (substring.startsWith(inputString)) {
                counts.set(substring, (counts.get(substring) || 0) + 1);
            }
        }
    }

    const pairs = Array.from(counts.entries());

    pairs.sort((a, b) => b[1] - a[1]);

    return pairs[0][0] || '';
}

function copyToClipboard(state, textArray, clipboardVisualBlock) {
    state.clipboardVisualBlock = clipboardVisualBlock;
    ncp.copy(textArray.join('\n'));
    state.clipboard = textArray.join('\n');
}

function insertIndentedRow(state) {
    const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row) : 0;
    state.data.splice(state.row, 0, ' '.repeat(indentLevel));
    state.col = indentLevel;
}

function findNonEmptyRow(state, start) {
    for (let i = start; i < state.data.length; i += 1) {
        if (!isEmptyRow(state, i)) return i;
    }
    return -1;
}

function findLastNonEmptyRow(state, start) {
    for (let i = start; i >= 0; i -= 1) {
        if (isEmptyRow(state, i)) return i + 1;
    }
    return 0;
}

function findNextEmptyRow(state, start) {
    for (let i = start; i < state.data.length; i += 1) {
        if (isEmptyRow(state, i)) return i;
    }
    return state.data.length;
}

function copyAndRemoveRows(state, start, end, includeEnd) {
    const newClipboard = [''];
    const loopEnd = includeEnd ? end : end - 1;
    for (let i = start; i <= loopEnd; i += 1) {
        newClipboard.push(state.data[i]);
    }
    copyToClipboard(state, newClipboard);
    state.data.splice(start, loopEnd - start + 1);
}

function copyRows(state, start, end, includeEnd) {
    const newClipboard = [''];
    const loopEnd = includeEnd ? end : end - 1;
    for (let i = start; i <= loopEnd; i += 1) {
        newClipboard.push(state.data[i]);
    }
    copyToClipboard(state, newClipboard);
}

function adjustRow(state) {
    if (state.row > state.data.length - 1) {
        state.row = state.data.length - 1;
    } else if (state.row < 0) {
        state.row = 0;
    }
}

function getSystemPaste(state) {
    if (state.allowCommandLogging) {
        return ncp.paste();
    } else {
        return state.clipboard;
    }
}

function isFile(filePath) {
    try {
        const stats = fs.statSync(filePath);
        return stats.isFile();
    } catch (err) {
        if (err.code === 'ENOENT') {
            return false;
        } else {
            throw err;
        }
    }
}

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
    let systemPaste = getSystemPaste(state);
    let newLine = false;
    if (systemPaste.startsWith('\n')) {
        systemPaste = systemPaste.substring(1);
        newLine = true;
    }
    systemPaste = systemPaste.split('\n');
    if (systemPaste.length > 0) {
        if (newLine) {
            for (let i = systemPaste.length - 1; i >= 0; i -= 1) {
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
    let systemPaste = getSystemPaste(state);
    let newLine = false;
    if (systemPaste.startsWith('\n')) {
        systemPaste = systemPaste.substring(1);
        newLine = true;
    }
    systemPaste = systemPaste.split('\n');
    if (systemPaste.length > 0) {
        if (newLine) {
            for (let i = systemPaste.length - 1; i >= 0; i -= 1) {
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
    if (state.mode === COMMAND) {
        screen.moveTo(
            state.commandCursorPosition + 1,
            0
        );
    } else {
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
}

function isRowHighlighted(state, i) {
    if (state.mode === VISUALLINE) {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else if (state.mode === VISUALBLOCK) {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else if (state.mode === VISUAL) {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else {
        return false;
    }
}

function isHighlighted(state, i, j) {
    if (state.mode === VISUALLINE) {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else if (state.mode === VISUALBLOCK) {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            if (state.visual.col <= state.col) {
                if (j >= state.visual.col && j <= state.col) {
                    return true;
                }
            } else if (state.visual.col > state.col) {
                if (j >= state.col && j <= state.visual.col) {
                    return true;
                }
            }
        }
        return false;
    } else if (state.mode === VISUAL) {
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

function getCommentString(file) {
    const extension = file.split('.').pop().toLowerCase();
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
    const commentString = getCommentString(state.file);
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

function getColorRow(replacing, replaceQuery, row, commentIndex, searching, searchQuery, isCurrentRow, col, commentString) {
    const output = [];
    let inString = false;
    let stringChar;
    let color = 'white';
    let disregardNext = false;
    let stop = false;
    for (let i = 0; i < row.length; i += 1) {
        if (i > 160) {
            output.push('red');
        } else {
            const s = row.substring(i, i + 1);
            if (inString) {
                color = 'cyan';
            }
            if (s === stringChar && stringChar !== undefined && !disregardNext) {
                inString = !inString;
                stringChar = undefined;
            } else if (s === '"' || s === '\'' || s === '`') {
                color = 'cyan';
                if (stringChar === undefined && commentString !== '' && commentString !== '<!--') {
                    inString = !inString;
                    stringChar = s;
                }
            } else if (!inString && (s === '(' || s === ')')) {
                color = 'yellow';
            } else if (!inString && (s === '[' || s === ']')) {
                color = 'green';
            } else if (!inString && (s === '{' || s === '}')) {
                color = 'magenta';
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

function saveFile(state) {
    fs.writeFileSync(state.file, state.data.join('\n'), (err) => {
        if (err) {
            console.log(err);
            process.exit();
        }
    });
}

function applySnapshot(state, index, backwards) {
    if (state.data.length < 10000) {
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
}

function createSnapshot(state) {
    if (state.data.length < 10000 && state.allowCommandLogging) {
        state.snapshots.splice(state.currentSnapshot + 1, state.snapshots.length - (state.currentSnapshot + 1));
        const oldData = [];
        for (let i = 0; i < state.data.length; i += 1) {
            oldData.push(state.data[i]);
        }
        state.snapshots.push({
            commandHistory: state.commandHistory,
            data: oldData,
            row: state.prevRow,
            col: state.prevCol,
            windowLine: state.windowLine
        });
        state.commandHistory = '';
        if (state.snapshots.length > 200) {
            state.snapshots.splice(0, state.snapshots.length - 200);
        }
        state.currentSnapshot = state.snapshots.length - 1;
        saveFile(state);
    }
}

function searchBackForString(state, string, acceptCurrent) {
    if (acceptCurrent && state.data[state.row].substring(state.col, state.col + string.length) === string) {
        return true;
    }
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

function getColorFromMode(mode) {
    if (mode === SHORTCUTS || mode === FILEFINDER || mode === GREP) {
        return 'white';
    } else if (mode === VISUAL || mode === VISUALLINE || mode === VISUALBLOCK) {
        return 'cyan';
    } else if (mode === TYPING || mode === MULTICURSOR) {
        return 'magenta';
    } else if (mode === HISTORY) {
        return 'blue';
    } else {
        return 'darkgrey';
    }
}

function renderStatusBar(state, screen) {
    if (state.mode === COMMAND) {
        screen.put({ attr: { color: 'white' } }, ':' + state.currentCommand);
    } else {
        screen.put({ attr: { color: 'green' } }, '/' + state.searchQuery + ' ');
        for (let i = 0; i < state.harpoonIndexes.length; i += 1) {
            screen.put({
                attr: {
                    color: i === state.harpoonIndex && state.file === state.files[state.harpoonIndexes[i]] ? 'yellow' : 'grey'
                }
            }, shortenFilePath(state.files[state.harpoonIndexes[i]]) + ' ');
        }
        screen.put({ attr: { color: getColorFromMode(state.mode) }, x: process.stdout.columns - (2 + state.file.length) }, '"' + state.file + '"');
    }
}

function getFileFinderColor(mode) {
    return mode === GREP ? 'green' : 'yellow';
}

function renderHistoryTree(state, screen) {
    const index = state.fileFinderIndex - Math.floor(process.stdout.rows / 2) > 0 ? state.fileFinderIndex - Math.floor(process.stdout.rows / 2) : 0;
    for (let i = index; i < state.fileFinderOutput.length && i < index + process.stdout.rows - 2; i += 1) {
        screen.put({
            attr: {
                color: state.fileFinderIndex === i ? 'blue' : 'white',
            },
            x: 0,
            wrap: false
        }, state.fileFinderOutput[i]);
        screen.put({ newLine: true }, '\n');
    }
    screen.moveTo(0, 0);
    screen.draw({ delta: true });
    screen.drawCursor();
}

function renderFileFinder(state, screen, mode) {
    const query = mode === GREP ? state.grepQuery : state.fileFinderQuery;
    const modeIndex = mode === GREP ? state.grepIndex : state.fileFinderIndex;
    const curPos = mode === GREP ? state.grepCursorPosition : state.fileFinderCursorPosition;
    screen.put({
        attr: {
            color: getFileFinderColor(mode),
        },
        x: 0,
        wrap: false
    }, '> ');
    screen.put({
        attr: {
            color: 'white',
        },
        wrap: false
    }, query);
    const index = modeIndex - Math.floor(process.stdout.rows / 2) > 0 ? modeIndex - Math.floor(process.stdout.rows / 2) : 0;
    for (let i = index; i < state.fileFinderOutput.length && i < index + process.stdout.rows - 2; i += 1) {
        screen.put({ newLine: true }, '\n');
        screen.put({
            attr: {
                color: modeIndex === i ? getFileFinderColor(mode) : 'white',
            },
            x: 0,
            wrap: false
        }, state.fileFinderOutput[i]);
    }
    screen.moveTo(
        curPos + 2,
        1
    );
    screen.draw({ delta: true });
    screen.drawCursor();
}

function renderSingleLine(state, screen, i, mergeSection, isContext) {
    let section = 0;
    let numColor = 'grey';
    let numBgColor;
    if (state.recording) {
        numColor = 'red';
    } else if (state.row === i) {
        numColor = 'white';
    }
    if (i === state.mark) {
        numBgColor = 'green';
        numColor = 'black';
    } else if (i === state.mark2) {
        numBgColor = 'cyan';
        numColor = 'black';
    }
    screen.put({
        attr: { color: numColor, bgColor: numBgColor, underline: isContext },
        x: 0
    }, (i + 1).toString().padStart(4) + ' ');
    if (isMergeConflictStart(state.data[i])) {
        section = 1;
    } else if (isMergeConflictMiddle(state.data[i])) {
        section = 2;
    } else if (isMergeConflictEnd(state.data[i])) {
        section = 0;
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
        state.col,
        getCommentString(state.file)
    );
    let displayRow = state.data[i];
    if (state.replacing) {
        displayRow = displayRow.replaceAll(state.searchQuery, state.replaceQuery);
    }
    for (let j = state.windowLineHorizontal; j < displayRow.length; j += 1) {
        let color = colorRow[j];
        let bgColor;
        if (section === 1 && !isMergeConflictStart(displayRow)) {
            color = 'red';
        } else if (section === 2 && !isMergeConflictMiddle(displayRow)) {
            color = 'green';
        } else if (isMergeConflictEnd(displayRow) || isMergeConflictMiddle(displayRow) || isMergeConflictStart(displayRow)) {
            color = 'blue';
        }
        if (colorRow[j] === 'search') {
            if (state.replacing || state.searching) {
                color = 'black';
                bgColor = 'cyan';
            }
        } else if (colorRow[j] === 'searchCurrent') {
            color = 'black';
            bgColor = 'magenta';
        }
        screen.put({
            attr: {
                color,
                bgColor,
                inverse: isHighlighted(state, i, j),
                underline: isContext
            },
            wrap: false
        }, displayRow.substring(j, j + 1));
        if (state.row === i && state.col - 1 === j && state.mode === TYPING) {
            const str = getCurrentWord(state.data[state.row], state.col);
            const replaceString = autocomplete(str, state.data);
            if (str.length !== 0) {
                screen.put({
                    attr: {
                        color: 'gray',
                        bgColor: 'black'
                    },
                    wrap: false
                }, replaceString.substring(str.length));
            }
        }
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
    return section;
}

function getIndent(state, row) {
    return state.data[row] !== undefined ? state.data[row].search(/\S|$/) : 0;
}

function getContextLines(state) {
    const contextLines = [];
    let indent = state.data[state.row] !== undefined ? state.data[state.row].search(/\S|$/) : 0;
    if (state.data[state.row].length === 0) {
        indent = 9999999; // if on blank line use indent of line above
    }
    for (let i = state.row; i >= 0; i -= 1) {
        if (state.data[i].length > 0 && getIndent(state, i) < indent) {
            if (i < state.windowLine) {
                contextLines.push(i);
            }
            indent = getIndent(state, i);
        }
    }
    return contextLines;
}

function renderWindowLines(state, screen, noCenterScreen, fullRefresh) {
    if (!noCenterScreen && !isOnScreen(state)) {
        centerScreen(state);
    }
    getWindowLineHorizontal(state);
    let mergeSection = 0;
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows) - 1; i += 1) {
        if (state.data[i] !== undefined) {
            mergeSection = renderSingleLine(state, screen, i, mergeSection);
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
    if (state.mode === HISTORY) {
        renderHistoryTree(state, screen);
    } else if (state.mode === GREP || state.mode === FILEFINDER) {
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
    state.mode = SHORTCUTS;
    state.row = 0;
    state.col = 0;
    state.windowLine = 0;
    state.windowLineHorizontal = 0;
    state.snapshots = [];
    state.currentSnapshot = 0;
    state.recording = false;
    state.mark = -1;
    state.mark2 = -1;
    state.data = getData(state.file);
    createSnapshot(state);
}

function evaluateCommand(state, term) {
    if (state.currentCommand === 'w') {
        saveFile(state);
        return false;
    } else if (state.currentCommand === 'wa') {
        saveFile(state);
        return false;
    } else if (state.currentCommand === 'x') {
        saveFile(state);
        term.fullscreen(false);
        process.exit(0);
    } else if (state.currentCommand === 'wq') {
        saveFile(state);
        term.fullscreen(false);
        process.exit(0);
    } else if (state.currentCommand === 'q') {
        term.fullscreen(false);
        process.exit(0);
    } else if (/gs\/(.*)\/(.*)\/g/.test(state.currentCommand)) {
        const match = /gs\/(.*?)\/(.*?)\/g/.exec(state.currentCommand);
        try {
            execSync(`git ls-files | xargs -I {} sed -i'' "s/${match[1]}/${match[2]}/g" "{}"`, { maxBuffer: 1024 * 1024 * 1000 });
            // eslint-disable-next-line no-empty
        } catch (e) {}
        refreshFile(state);
        createSnapshot(state);
        return true;
    } else if (/s\/(.*)\/(.*)\/g/.test(state.currentCommand)) {
        const match = /s\/(.*?)\/(.*?)\/g/.exec(state.currentCommand);
        state.data[state.row] = state.data[state.row].replaceAll(new RegExp(match[1], 'g'), match[2]);
        createSnapshot(state);
        return true;
    } else if (/%s\/(.*)\/(.*)\/g/.test(state.currentCommand)) {
        const match = /%s\/(.*?)\/(.*?)\/g/.exec(state.currentCommand);
        for (let i = 0; i < state.data.length; i += 1) {
            state.data[i] = state.data[i].replaceAll(new RegExp(match[1], 'g'), match[2]);
        }
        createSnapshot(state);
        return true;
    } else if (state.currentCommand === 'set ts=2') {
        state.indentAmount = 2;
        return false;
    } else if (state.currentCommand === 'set ts=4') {
        state.indentAmount = 4;
        return false;
    } else if (!Number.isNaN(state.currentCommand)) {
        const num = parseInt(state.currentCommand);
        if (num > 0 && num < state.data.length) {
            state.row = num - 1;
        }
        return false;
    } else if (state.currentCommand === 'qa') {
        term.fullscreen(false);
        process.exit(0);
    } else if (state.currentCommand === 'qa!') {
        term.fullscreen(false);
        process.exit(0);
    } else if (state.currentCommand === 'q!') {
        term.fullscreen(false);
        process.exit(0);
    }
}

function createState(state) {
    const snapshotsCopy = state.snapshots.map((snapshot) => JSON.parse(JSON.stringify(snapshot)));
    return {
        row: state.row,
        col: state.col,
        windowLine: state.windowLine,
        windowLineHorizontal: state.windowLineHorizontal,
        currentSnapshot: state.currentSnapshot,
        snapshots: snapshotsCopy,
        mark: state.mark,
        mark2: state.mark2,
        prevRow: state.prevRow,
        prevCol: state.prevCol,
    };
}

function updateStateFromFilePosition(state, fileIndex, lineNum) {
    const pos = state.storePosition[fileIndex];
    state.row = lineNum !== 0 ? lineNum - 1 : pos.row;
    state.col = pos.col;
    state.windowLine = pos.windowLine;
    state.windowLineHorizontal = pos.windowLineHorizontal;
    state.currentSnapshot = pos.currentSnapshot;
    state.snapshots = pos.snapshots;
    state.mark = pos.mark;
    state.mark2 = pos.mark2;
    state.prevRow = pos.prevRow;
    state.prevCol = pos.prevCol;
}

function updateFileState(state, newFile, lineNum) {
    const fileExistsInState = state.files.includes(state.file);
    const newState = createState(state);
    if (fileExistsInState) {
        state.storePosition[state.fileIndex] = newState;
    } else {
        state.storePosition.push(newState);
    }
    state.file = newFile;
    state.fileIndex = fileExistsInState ? state.files.indexOf(state.file) : state.files.length - 1;
    changeFile(state);
    if (lineNum !== 0) {
        state.row = lineNum - 1;
        state.col = 0;
    }
}

function processFile(state, newFile, lineNum, fileExists) {
    if (fileExists) {
        if (!state.files.includes(newFile)) {
            state.file = newFile;
            state.files.push(state.file);
            updateFileState(state, newFile, lineNum);
        } else {
            updateFileState(state, newFile, lineNum);
            updateStateFromFilePosition(state, state.fileIndex, lineNum);
        }
    }
}

function cleanup(state, key, log, newCommand, snapshot, resetPrevKeys) {
    if (log) {
        logCommand(newCommand, state, key);
    }
    if (snapshot) {
        createSnapshot(state);
    }
    if (resetPrevKeys) {
        state.previousKeys = '';
    }
}

function isValidSearch(query, file) {
    let counter = 0;
    for (let i = 0; i < query.length; i += 1) {
        for (let j = counter; j <= file.length; j += 1) {
            if (j === file.length) {
                return false;
            }
            if (query.charAt(i) === file.charAt(j)) {
                counter = j;
                break;
            }
        }
    }
    return true;
}

function setFileSearchOutput(state) {
    if (state.gitFinding) {
        state.fileFinderFileCache = execSync('fd -t f --hidden -E .git').toString();
    } else {
        state.fileFinderFileCache = execSync('find * -type f').toString();
    }
}

function calcGrepOutput(state) {
    if (state.grepQuery.length !== 0) {
        state.fileFinderOutput = execSync(`git grep -n "${state.grepQuery}" || true`, { maxBuffer: 1024 * 1024 * 1000 }).toString().split('\n');
    } else {
        state.fileFinderOutput = [];
    }
}

function findLongestSubstringLength(str, query) {
    let longest = 0;

    for (let i = 0; i < str.length; i++) {
        for (let j = i; j < str.length; j++) {
            const sub = str.substring(i, j + 1);
            if (query.includes(sub) && sub.length > longest) {
                longest = sub.length;
            }
        }
    }

    return longest;
}

function sortOutputBySubstring(state, query) {
    state.fileFinderOutput.sort((a, b) => {
        const lenA = findLongestSubstringLength(a, query);
        const lenB = findLongestSubstringLength(b, query);

        return lenB - lenA || a.localeCompare(b);
    });
}

function calcFileFinderOutput(state) {
    state.fileFinderOutput = state.fileFinderFileCache.split('\n').filter((file) => file !== state.file && isValidSearch(state.fileFinderQuery, file));
    sortOutputBySubstring(state, state.fileFinderQuery);
}

export {
    calcGrepOutput,
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
    getData,
    evaluateCommand,
    processFile,
    getContextLines,
    getSystemPaste,
    calcFileFinderOutput,
    findNonEmptyRow,
    findLastNonEmptyRow,
    findNextEmptyRow,
    copyRows,
    copyAndRemoveRows,
    insertIndentedRow,
    adjustRow,
    cleanup,
    getCurrentWord,
    autocomplete,
    setFileSearchOutput,
    isFile,
};
