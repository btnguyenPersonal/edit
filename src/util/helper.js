/* eslint-disable import/no-cycle */
import path from 'path';
import ncp from 'copy-paste';
import { execSync } from 'child_process';
import fs from 'fs';
import {
    SHORTCUTS,
} from './modes.js';

function isInGit() {
    try {
        return execSync('git rev-parse --is-inside-work-tree').toString().includes('true');
    } catch (err) {
        return false;
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

function tryPaths(input) {
    const extensions = [
        '',
        '.rs',
        '.cpp',
        '.c',
        '.py',
        '.js',
        '.jsx',
        '.ts',
        '.tsx',
        '/index.js',
        '/index.jsx',
        '/index.ts',
        '/index.tsx',
    ];
    for (let i = 0; i < extensions.length; i += 1) {
        if (isFile(input + extensions[i])) {
            return input + extensions[i];
        }
    }
    return undefined;
}

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
            if (substring.startsWith(inputString) && substring !== inputString) {
                counts.set(substring, (counts.get(substring) || 0) + 1);
            }
        }
    }

    const pairs = Array.from(counts.entries());

    pairs.sort((a, b) => b[1] - a[1]);

    return pairs[0] ? pairs[0][0] : inputString;
}

function copyToClipboard(state, textArray, clipboardVisualBlock) {
    state.clipboardVisualBlock = clipboardVisualBlock;
    ncp.copy(textArray.join('\n'));
    state.clipboard = textArray.join('\n');
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

function getData(filepath) {
    try {
        return (fs.readFileSync(filepath, 'utf-8')).split('\n');
    } catch (e) {
        console.log(filepath + ' not found\nusage: edit [file]');
        process.exit();
    }
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

function sortLines(state) {
    const lines = [];
    let begin = state.row;
    let end = state.visual.row;
    if (begin > end) {
        begin = state.visual.row;
        end = state.row;
    }
    for (let i = begin; i <= end; i += 1) {
        lines.push(state.data[i]);
    }
    return lines.sort();
}

function isWritable(s) {
    return ' qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > -1;
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
        case 'md':
            commentString = '#';
            break;
        case 'css':
            commentString = '/*';
            break;
        case 'html':
        case 'xml':
        case 'markdown':
            commentString = '<!--';
            break;
        default:
            break;
    }
    return commentString;
}

function isOnScreen(state) {
    return state.row >= state.windowLine && state.row <= state.windowLine + process.stdout.rows - 1;
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

function saveFile(state) {
    fs.writeFileSync(state.file, state.data.join('\n'), (err) => {
        if (err) {
            console.log(err);
            process.exit();
        }
    });
}

function centerScreen(state) {
    state.windowLine = state.row - Math.floor(process.stdout.rows / 2) >= 0 ? state.row - Math.floor(process.stdout.rows / 2) : 0;
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
    if (!state.fileStack.includes(state.file)) {
        state.fileStack.splice(state.fileStackIndex + 1, state.fileStack.length - (state.fileStackIndex + 1));
        state.fileStack.push(state.file);
        state.fileStackIndex = state.fileStack.length - 1;
    }
    createSnapshot(state);
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
    state.row = lineNum !== -1 ? lineNum - 1 : pos.row;
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
    if (lineNum !== -1) {
        state.row = lineNum - 1;
        state.col = 0;
    }
}

function processFile(state, newFile, lineNum) {
    if (!state.files.includes(newFile)) {
        state.file = newFile;
        state.files.push(state.file);
        updateFileState(state, newFile, lineNum);
    } else {
        updateFileState(state, newFile, lineNum);
        updateStateFromFilePosition(state, state.fileIndex, lineNum);
    }
}

function cleanup(state, key, log, newCommand, snapshot, resetPrevKeys) {
    if (state.row > state.data.length - 1) {
        state.row = state.data.length - 1;
    }
    if (state.row < 0) {
        state.row = 0;
    }
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

function findLongestSubstringLength(str, query) {
    const dp = Array.from({ length: str.length + 1 }, () => Array(query.length + 1).fill(0));
    let longest = 0;

    for (let i = 1; i <= str.length; i += 1) {
        for (let j = 1; j <= query.length; j += 1) {
            if (str[i - 1] === query[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                longest = Math.max(longest, dp[i][j]);
            }
        }
    }

    return longest;
}

function isValidSearch(query, file, lengthCache) {
    let counter = 0;
    for (let i = 0; i < query.length; i += 1) {
        for (let j = counter; j <= file.length; j += 1) {
            if (j === file.length) {
                return false;
            }
            if (query.charAt(i) === query.charAt(i).toUpperCase() && query.charAt(i) === file.charAt(j)) {
                counter = j;
                break;
            } else if (query.charAt(i) === query.charAt(i).toLowerCase()
                && (query.charAt(i) === file.charAt(j).toUpperCase() || query.charAt(i) === file.charAt(j).toLowerCase())) {
                counter = j;
                break;
            }
        }
    }
    lengthCache.set(file, findLongestSubstringLength(file, query));
    return true;
}

function createFolderIfNotExists(state) {
    if (state.newFile.includes('/')) {
        let createdPath = state.newFile.split('/');
        createdPath.pop();
        createdPath = createdPath.join('/');
        fs.mkdirSync(state.selectedFolder + '/' + createdPath, { recursive: true });
    }
}

function setFileSearchOutput(state, useGit) {
    if (useGit) {
        if (state.git) {
            state.fileFinderFileCache = execSync('fd -t f --hidden -E .git', { maxBuffer: 1024 * 1024 * 1000 }).toString().split('\n');
        } else {
            state.status = 'not in git repository';
        }
    } else {
        state.fileFinderFileCache = execSync(
            'find . -type f -not -path "./.git/*" -not -path "./node_modules/*"',
            { maxBuffer: 1024 * 1024 * 1000 }
        ).toString().split('\n').map(line => line.substring(2));
    }
    if (state.fileFinderFileCache.length > 1000) {
        state.skipSortingFileFinder = true;
    }
}

function calcGrepOutput(state) {
    if (state.grepQuery.length !== 0) {
        let sanitized = state.grepQuery.replace(/\\/g, '\\\\\\\\');
        sanitized = sanitized.replace(/"/g, '\\"');
        const ignoredFiles = '":!package-lock.json" ":!pnpm-lock.yaml" ":!bun.lockb" ":!yarn.lock"';
        if (state.git) {
            state.fileFinderOutput = execSync(
                `git grep -n --untracked "${sanitized}" -- ${ignoredFiles} || true`,
                { maxBuffer: 1024 * 1024 * 1000 }
            ).toString().split('\n');
        } else {
            state.fileFinderOutput = execSync(
                `grep -nr "${sanitized}" . || true`,
                { maxBuffer: 1024 * 1024 * 1000 }
            ).toString().split('\n');
        }
        state.fileFinderOutput = state.fileFinderOutput.filter(line => line.length > 0);
    } else {
        state.fileFinderOutput = [];
    }
}

function sortOutputBySubstring(state, lengthCache) {
    state.fileFinderOutput.sort((a, b) => {
        const lenA = lengthCache.get(a);
        const lenB = lengthCache.get(b);

        return lenB - lenA || a.localeCompare(b);
    });
}

function calcFileFinderOutput(state) {
    if (state.fileFinderFileCache.length > 1) {
        const lengthCache = new Map();
        state.fileFinderOutput = state.fileFinderFileCache.filter(
            (file) => file !== state.file && file.trim() !== '' && isValidSearch(state.fileFinderQuery, file, lengthCache)
        );
        sortOutputBySubstring(state, lengthCache);
    }
}

function setFileExplorerFiles(state) {
    const output = execSync('fd -t f --hidden -E .git', { maxBuffer: 1024 * 1024 * 1000 }).toString();
    state.fileExplorerCopyOutput = output.split('\n');
    state.fileExplorerOutput = output.split('\n');
}

function calcFileExplorerOutput(state, keepPosition) {
    setFileExplorerFiles(state);

    const existingDirs = new Set();
    const newOutput = ['__DIR.'];

    state.fileExplorerOutput.forEach((filePath) => {
        const splitPath = filePath.split('/');
        let indent = 0;
        let fullPath = '';

        splitPath.forEach((segment, idx) => {
            if (idx < splitPath.length - 1) {
                fullPath += (fullPath ? '/' : '') + segment;
                const dirKey = '__DIR' + ' '.repeat(indent) + segment;
                if (!existingDirs.has(fullPath)) {
                    newOutput.push(dirKey);
                    existingDirs.add(fullPath);
                }
                indent += 2;
            } else {
                newOutput.push(' '.repeat(indent) + segment);
            }
        });

        if (!keepPosition && filePath === state.file) {
            state.fileExplorerIndex = newOutput.length - 1;
        }
    });

    state.fileExplorerOutput = newOutput;
}

function getFolderFromExplorer(state) {
    let dir = '';
    let indent = state.fileExplorerOutput[state.fileExplorerIndex].substring(5).search(/\S|$/);
    for (let i = state.fileExplorerIndex - 1; i >= 0; i -= 1) {
        if (state.fileExplorerOutput[i].includes('__DIR') && state.fileExplorerOutput[i].substring(5).search(/\S|$/) < indent) {
            dir = state.fileExplorerOutput[i].substring(5).trim() + '/' + dir;
            indent = state.fileExplorerOutput[i].substring(5).search(/\S|$/);
        }
    }
    return dir + state.fileExplorerOutput[state.fileExplorerIndex].substring(5).trim();
}

function findCurrentIndentLevel(state, prevLine, currentLine) {
    if (prevLine.replace(new RegExp(`${getCommentString(state.file)}.*$`, 'gm'), '').trimEnd() !== '') {
        prevLine = prevLine.replace(new RegExp(`${getCommentString(state.file)}.*$`, 'gm'), '').trimEnd();
    }
    currentLine = currentLine.replace(new RegExp(`${getCommentString(state.file)}.*$`, 'gm'), '').trimEnd();
    let indent = prevLine.search(/\S|$/);
    if (currentLine.startsWith(getCommentString(state.file))) {
        return indent;
    }
    const prevLineSelfClosingTag = /<[^/]*\/>/.test(prevLine);
    const prevLineOpensTag = /<[^/]*>/.test(prevLine) && !prevLineSelfClosingTag;
    const prevLineClosesTag = /<\/[^>]+>/.test(prevLine);
    const currentLineClosesTag = /<\/[^>]+>/.test(currentLine);
    if (prevLineOpensTag) {
        indent += state.indentAmount;
        return Math.max(0, indent);
    }
    if (prevLineClosesTag || currentLineClosesTag || currentLine.trim() === '</>') {
        indent -= state.indentAmount;
        return Math.max(0, indent);
    }
    if (prevLine.endsWith('[') || prevLine.endsWith('(') || prevLine.endsWith('{')) {
        indent += state.indentAmount;
    }
    if (currentLine.startsWith(']') || currentLine.startsWith(')') || currentLine.startsWith('}')) {
        indent -= state.indentAmount;
    }
    return Math.max(0, indent);
}

function findLastIndentLevel(state, start) {
    for (let i = start - 1; i >= 0; i -= 1) {
        if (state.data[i].trimEnd() !== '') {
            return state.data[i];
        }
    }
    return '';
}

function getFormattedLines(state, start, end) {
    const lines = [];
    for (let i = start; i <= end; i += 1) {
        lines.push(state.data[i].trim());
    }
    let lastLine = findLastIndentLevel(state, start);
    for (let i = 0; i < lines.length; i += 1) {
        const indent = findCurrentIndentLevel(state, lastLine, lines[i]);
        if (lines[i] !== '') {
            lines[i] = ' '.repeat(indent) + lines[i];
            lastLine = lines[i];
        }
    }
    return lines;
}

function getFileFromExplorer(state) {
    let numDir = 0;
    for (let i = state.fileExplorerIndex; i >= 0; i -= 1) {
        if (state.fileExplorerOutput[i].includes('__DIR')) {
            numDir += 1;
        }
    }
    return state.fileExplorerCopyOutput[state.fileExplorerIndex - numDir];
}

function insertIndentedRow(state, insertUnder) {
    if (insertUnder) {
        const indentLevel = findCurrentIndentLevel(state, findLastIndentLevel(state, state.row + 1), '');
        state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
        state.col = indentLevel;
    } else {
        const indentLevel = findCurrentIndentLevel(state, findLastIndentLevel(state, state.row), '');
        state.data.splice(state.row, 0, ' '.repeat(indentLevel));
        state.col = indentLevel;
    }
}

function evaluateCommand(state, term) {
    if (state.currentCommand.startsWith('e ')) {
        const newFile = state.currentCommand.substring(2);
        const validPath = tryPaths(path.join(process.cwd(), newFile));
        if (validPath) {
            processFile(state, validPath, -1);
        }
    } else if (state.currentCommand === 'w') {
        saveFile(state);
    } else if (state.currentCommand === 'wa') {
        saveFile(state);
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
    } else if (/^gs\/(.*)\/(.*)\/g$/.test(state.currentCommand)) {
        const match = /gs\/(.*?)\/(.*?)\/g/.exec(state.currentCommand);
        try {
            execSync(`git ls-files | xargs -I {} sed -i'' "s/${match[1]}/${match[2]}/g" "{}"`, { maxBuffer: 1024 * 1024 * 1000 });
        } catch (e) {
            state.status = 'not in git project';
        }
        refreshFile(state);
        createSnapshot(state);
    } else if (/^s\/(.*)\/(.*)\/g$/.test(state.currentCommand)) {
        const match = /s\/(.*?)\/(.*?)\/g/.exec(state.currentCommand);
        state.data[state.row] = state.data[state.row].replaceAll(new RegExp(match[1], 'g'), match[2]);
        createSnapshot(state);
    } else if (/^%s\/(.*)\/(.*)\/g$/.test(state.currentCommand)) {
        const match = /%s\/(.*?)\/(.*?)\/g/.exec(state.currentCommand);
        for (let i = 0; i < state.data.length; i += 1) {
            state.data[i] = state.data[i].replaceAll(new RegExp(match[1], 'g'), match[2]);
        }
        createSnapshot(state);
    } else if (state.currentCommand === 'set ts=2') {
        state.indentAmount = 2;
    } else if (state.currentCommand === 'set ts=4') {
        state.indentAmount = 4;
    } else if (!Number.isNaN(state.currentCommand)) {
        const num = parseInt(state.currentCommand);
        if (num > 0 && num < state.data.length) {
            state.row = num - 1;
        }
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

export {
    adjustRow,
    applySnapshot,
    autocomplete,
    calcFileExplorerOutput,
    calcFileFinderOutput,
    calcGrepOutput,
    centerScreen,
    cleanup,
    copyAndRemoveRows,
    copyRows,
    createFolderIfNotExists,
    copyToClipboard,
    createSnapshot,
    evaluateCommand,
    isInGit,
    findCurrentIndentLevel,
    getContextLines,
    getCurrentWord,
    getData,
    getFileFromExplorer,
    getFolderFromExplorer,
    getFormattedLines,
    getSystemPaste,
    insertIndentedRow,
    isAlphaNumeric,
    isFile,
    isNumeric,
    isOnScreen,
    isWritable,
    logCommand,
    pasteFromClipboardAfter,
    pasteFromClipboardBefore,
    processFile,
    refreshFile,
    saveFile,
    searchBackForString,
    searchForString,
    searchForStringNoWrap,
    setFileSearchOutput,
    sortLines,
    findLastIndentLevel,
    getCommentString,
    trimTrailingWhitespace,
    tryPaths,
};
