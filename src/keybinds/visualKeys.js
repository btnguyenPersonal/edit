/* eslint-disable import/no-cycle */
import { execSync } from 'child_process';
import ncp from 'copy-paste';
import path from 'path';
import fs from 'fs';
import {
    renderScreen,
    createSnapshot,
    isWritable,
    searchForString,
    centerScreen,
    changeFile,
    logCommand
} from '../util/helper.js';
import {
    up,
    down,
    left,
    right,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    upHalfScreen,
    downHalfScreen,
    getInsideOfIndentLevel,
    getCoorsInsideCharSame,
    getCoorsInsideCharDiff,
    getCoorsInsideWord,
    setVisualHighlight,
    copyInVisual,
    getInVisual,
    deleteInVisual,
    endOfLine,
    findForward,
    findBackward,
    increaseIndentLevel,
    decreaseIndentLevel,
    getIndentLevelFrom,
    setAroundVisualHighlight,
    getIndentLevel,
    toForward,
    toBackward,
    isEmptyRow,
    isCommented,
    toggleComment,
} from '../util/movement.js';

function handleVisualKeys(key, state, screen) {
    if (state.previousKeys === 'i') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            setVisualHighlight(state, beginning, end);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            setVisualHighlight(state, beginning, end);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            setVisualHighlight(state, beginning, end);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            setVisualHighlight(state, beginning, end);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            setVisualHighlight(state, beginning, end);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            setVisualHighlight(state, beginning, end);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            setVisualHighlight(state, beginning, end);
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning;
                state.row = end;
                state.col = 0;
                state.mode = 'V';
            } else {
                state.mode = 'n';
            }
        } else if (key === 'p') {
            for (let i = state.row; i < state.data.length; i += 1) {
                if (!isEmptyRow(state, i)) {
                    state.row = i;
                    state.col = 0;
                    break;
                }
            }
            state.visualLine.row = state.row;
            for (let i = state.row; i >= 0; i -= 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.visualLine.row = i;
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.row = i;
                }
            }
            state.mode = 'V';
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'a') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            setAroundVisualHighlight(state, beginning + 1, end);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            setAroundVisualHighlight(state, beginning, end);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            setAroundVisualHighlight(state, beginning, end);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            setAroundVisualHighlight(state, beginning, end);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            setAroundVisualHighlight(state, beginning, end);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            setAroundVisualHighlight(state, beginning, end);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            setAroundVisualHighlight(state, beginning, end);
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning - 1;
                state.row = end + 1;
                state.col = 0;
                state.mode = 'V';
            } else {
                state.mode = 'n';
            }
        } else if (key === 'p') {
            for (let i = state.row; i < state.data.length; i += 1) {
                if (!isEmptyRow(state, i)) {
                    state.row = i;
                    state.col = 0;
                    break;
                }
            }
            state.visualLine.row = state.row;
            for (let i = state.row; i >= 0; i -= 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.visualLine.row = i;
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    state.row = i;
                    break;
                } else {
                    state.row = i;
                }
            }
            state.mode = 'V';
        }
        state.previousKeys = '';
    } else if (key === 'CTRL_U') {
        upHalfScreen(state);
    } else if (key === 'CTRL_D') {
        downHalfScreen(state);
    } else if (key === 'UP' || key === 'k') {
        up(state);
    } else if (key === 'DOWN' || key === 'j') {
        down(state);
    } else if (key === 'LEFT' || key === 'h') {
        left(state);
    } else if (key === 'RIGHT' || key === 'l') {
        right(state);
    } else if (key === 'w') {
        state.col = getCoorBeginningNextWord(state);
    } else if (key === 'b') {
        state.col = getCoorBeginningLastWord(state);
    } else if (key === '$') {
        state.col = endOfLine(state, state.row);
    } else if (key === '0') {
        state.col = 0;
    } else if (key === '^') {
        state.col = firstNonSpace(state, state.row);
    } else if (key === 'p' || key === 'P') {
        let systemPaste = ncp.paste();
        if (systemPaste.startsWith('\n')) {
            systemPaste = systemPaste.substring(1);
        }
        systemPaste = systemPaste.split('\n');
        if (systemPaste.length > 0) {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.data[state.row] = state.data[state.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(state.visual.col + 1);
                }
            } else if (state.row > state.visual.row) {
                state.data[state.visual.row] = state.data[state.visual.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                state.data.splice(state.visual.row + 1, state.row - state.visual.row);
                state.row = state.visual.row;
                state.col = state.visual.col;
            } else if (state.row < state.visual.row) {
                state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.visual.row].substring(state.visual.col + 1);
                state.data.splice(state.row + 1, state.visual.row - state.row);
            }
            const cutoff = state.data[state.row].substring(state.col);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + systemPaste[0];
            let counterRow = state.row;
            for (let i = systemPaste.length - 1; i >= 1; i -= 1) {
                state.data.splice(state.row + 1, 0, systemPaste[i]);
                counterRow += 1;
            }
            state.data[counterRow] += cutoff;
        }
        state.mode = 'n';
        createSnapshot(state);
    } else if (state.previousKeys === 'g' && key === 'f') {
        const newFile = getInVisual(state);
        const currentDirectory = path.dirname(state.file);
        let convertedPath = path.join(currentDirectory, newFile);
        let fileExists = fs.existsSync(convertedPath);
        if (!fileExists) {
            convertedPath += '.js';
            fileExists = fs.existsSync(convertedPath);
        }
        if (!fileExists) {
            convertedPath = convertedPath.slice(-3);
            convertedPath += '/index.js';
            fileExists = fs.existsSync(convertedPath);
        }
        if (fileExists) {
            if (!state.files.includes(convertedPath)) {
                state.file = convertedPath;
                state.files.push(state.file);
                const snapshotsCopy = [];
                for (let i = 0; i < state.snapshots.length; i += 1) {
                    snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                }
                if (state.files.includes(state.file)) {
                    state.storePosition[state.fileIndex] = {
                        row: state.row,
                        col: state.col,
                        windowLine: state.windowLine,
                        windowLineHorizontal: state.windowLineHorizontal,
                        snapshots: snapshotsCopy,
                        savePoint: state.savePoint,
                        mark: state.mark,
                        prevRow: state.prevRow,
                        prevCol: state.prevCol,
                    };
                } else {
                    state.storePosition.push({
                        row: state.row,
                        col: state.col,
                        windowLine: state.windowLine,
                        windowLineHorizontal: state.windowLineHorizontal,
                        currentSnapshot: state.currentSnapshot,
                        snapshots: snapshotsCopy,
                        savePoint: state.savePoint,
                        mark: state.mark,
                        prevRow: state.prevRow,
                        prevCol: state.prevCol,
                    });
                }
                state.fileIndex = state.files.length - 1;
                changeFile(state);
            } else {
                const snapshotsCopy = [];
                for (let i = 0; i < state.snapshots.length; i += 1) {
                    snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                }
                if (state.files.includes(state.file)) {
                    state.storePosition[state.fileIndex] = {
                        row: state.row,
                        col: state.col,
                        windowLine: state.windowLine,
                        windowLineHorizontal: state.windowLineHorizontal,
                        snapshots: snapshotsCopy,
                        savePoint: state.savePoint,
                        mark: state.mark,
                        prevRow: state.prevRow,
                        prevCol: state.prevCol,
                    };
                } else {
                    state.storePosition.push({
                        row: state.row,
                        col: state.col,
                        windowLine: state.windowLine,
                        windowLineHorizontal: state.windowLineHorizontal,
                        currentSnapshot: state.currentSnapshot,
                        snapshots: snapshotsCopy,
                        savePoint: state.savePoint,
                        mark: state.mark,
                        prevRow: state.prevRow,
                        prevCol: state.prevCol,
                    });
                }
                state.file = convertedPath;
                state.fileIndex = state.files.indexOf(state.file);
                changeFile(state);
                const pos = state.storePosition[state.fileIndex];
                state.row = pos.row;
                state.col = pos.col;
                state.windowLine = pos.windowLine;
                state.windowLineHorizontal = pos.windowLineHorizontal;
                state.currentSnapshot = pos.currentSnapshot;
                state.snapshots = pos.snapshots;
                state.savePoint = pos.savePoint;
                state.mark = pos.mark;
                state.prevRow = pos.prevRow;
                state.prevCol = pos.prevCol;
            }
        }
    } else if (key === '#') {
        state.fileFinderQuery = getInVisual(state);
        state.mode = 'g';
        let output = '';
        if (state.fileFinderQuery.length !== 0) {
            output = execSync(
                `git grep -n "${state.fileFinderQuery}" || true`,
                { maxBuffer: 1024 * 1024 * 1000 }
            ).toString();
        }
        state.fileFindingOutput = output.split('\n');
    } else if (key === '*') {
        state.searchQuery = getInVisual(state);
        state.mode = 'n';
        if (state.searchQuery !== '') {
            state.col += state.searchQuery.length + 1;
            searchForString(state, state.searchQuery);
            state.searching = true;
            centerScreen(state);
        }
    } else if (key === 'y') {
        copyInVisual(state);
        if (state.row >= state.visual.row) {
            state.row = state.visual.row;
        }
        if (state.col >= state.visual.col) {
            state.col = state.visual.col;
        }
        state.mode = 'n';
    } else if (key === 'x') {
        deleteInVisual(state);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'd') {
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'c') {
        if (state.previousKeys === 'g') {
            let areAllCommented = true;
            if (state.row >= state.visual.row) {
                for (let i = state.visual.row; i <= state.row; i += 1) {
                    if (state.data[i].length !== 0 && !isCommented(state, i)) {
                        areAllCommented = false;
                        break;
                    }
                }
                if (areAllCommented) {
                    for (let i = state.visual.row; i <= state.row; i += 1) {
                        toggleComment(state, i, undefined, false);
                    }
                } else {
                    for (let i = state.visual.row; i <= state.row; i += 1) {
                        toggleComment(state, i, firstNonSpace(state, state.visual.row), true);
                    }
                }
                state.row = state.visual.row;
            } else if (state.row < state.visual.row) {
                for (let i = state.row; i <= state.visual.row; i += 1) {
                    if (state.data[i].length !== 0 && !isCommented(state, i)) {
                        areAllCommented = false;
                        break;
                    }
                }
                if (areAllCommented) {
                    for (let i = state.row; i <= state.visual.row; i += 1) {
                        toggleComment(state, i, undefined, false);
                    }
                } else {
                    for (let i = state.row; i <= state.visual.row; i += 1) {
                        toggleComment(state, i, firstNonSpace(state, state.row), true);
                    }
                }
            }
            state.mode = 'n';
            createSnapshot(state);
            state.previousKeys = '';
        } else {
            copyInVisual(state);
            deleteInVisual(state);
            state.mode = 'i';
        }
    } else if (key === '=') {
        if (state.row >= state.visual.row) {
            for (let i = state.visual.row; i <= state.row; i += 1) {
                let indentLevel = i - 1 < 0 ? 0 : getIndentLevelFrom(state, i - 1);
                if (state.data[i].trim().startsWith(')') || state.data[i].trim().startsWith('}')) {
                    indentLevel = indentLevel - state.indentAmount >= 0 ? indentLevel - state.indentAmount : 0;
                }
                state.data[i] = ' '.repeat(indentLevel) + state.data[i].trim();
            }
        } else if (state.row < state.visual.row) {
            for (let i = state.row; i <= state.visual.row; i += 1) {
                let indentLevel = i - 1 < 0 ? 0 : getIndentLevelFrom(state, i - 1);
                if (state.data[i].trim().startsWith(')') || state.data[i].trim().startsWith('}')) {
                    indentLevel = indentLevel - state.indentAmount >= 0 ? indentLevel - state.indentAmount : 0;
                }
                state.data[i] = ' '.repeat(indentLevel) + state.data[i].trim();
            }
        }
        state.mode = 'n';
    } else if (key === '<') {
        if (state.row >= state.visual.row) {
            for (let i = state.visual.row; i <= state.row; i += 1) {
                decreaseIndentLevel(state, i);
            }
        } else if (state.row < state.visual.row) {
            for (let i = state.row; i <= state.visual.row; i += 1) {
                decreaseIndentLevel(state, i);
            }
        }
        state.col = firstNonSpace(state, state.row);
        state.row = state.visual.row;
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === '>') {
        if (state.row >= state.visual.row) {
            for (let i = state.visual.row; i <= state.row; i += 1) {
                increaseIndentLevel(state, i);
            }
            state.row = state.visual.row;
        } else if (state.row < state.visual.row) {
            for (let i = state.row; i <= state.visual.row; i += 1) {
                increaseIndentLevel(state, i);
            }
        }
        state.col = firstNonSpace(state, state.row);
        state.mode = 'n';
        createSnapshot(state);
    } else if (state.previousKeys === 'T') {
        if (isWritable(key)) {
            state.col = toBackward(state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 't') {
        if (isWritable(key)) {
            state.col = toForward(state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'F') {
        if (isWritable(key)) {
            state.col = findBackward(state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'f') {
        if (isWritable(key)) {
            state.col = findForward(state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'i' || key === 'g' || key === 'a') {
        state.previousKeys += key;
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleVisualKeys
};
