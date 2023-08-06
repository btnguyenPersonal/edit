/* eslint-disable import/no-cycle */
import path from 'path';
import {
    renderScreen,
    createSnapshot,
    isWritable,
    searchForString,
    centerScreen,
    isFile,
    getSystemPaste,
    processFile,
    calcGrepOutput,
    findNextEmptyRow,
    findLastNonEmptyRow,
    logCommand
} from '../util/helper.js';
import {
    TYPING,
    SHORTCUTS,
    GREP,
    VISUALLINE,
    VISUALBLOCK,
    SEARCH
} from '../util/modes.js';
import {
    up,
    down,
    left,
    right,
    topOfFile,
    bottomOfFile,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    upHalfScreen,
    downHalfScreen,
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
    toForward,
    toBackward,
    isCommented,
    toggleComment,
    matchIt,
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
        } else if (key === 'p') {
            state.row = findLastNonEmptyRow(state, state.row);
            state.visual.row = findNextEmptyRow(state, state.row + 1) - 1;
            state.mode = VISUALLINE;
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
        } else if (key === 'p') {
            state.row = findLastNonEmptyRow(state, state.row);
            state.visual.row = findNextEmptyRow(state, state.row + 1);
            state.mode = VISUALLINE;
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
    } else if (key === 'g') {
        if (state.previousKeys === 'g') {
            topOfFile(state);
            state.previousKeys = '';
        } else {
            state.previousKeys = 'g';
        }
    } else if (key === 'G') {
        bottomOfFile(state);
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
        let systemPaste = getSystemPaste(state);
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
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (state.previousKeys === 'g' && key === 'f') {
        const newFile = getInVisual(state);
        const currentDirectory = path.dirname(state.file);
        let convertedPath = path.join(currentDirectory, newFile);
        let fileExists = isFile(convertedPath);
        if (!fileExists) {
            convertedPath += '.js';
            fileExists = isFile(convertedPath);
        }
        if (!fileExists) {
            convertedPath = convertedPath.slice(0, -3);
            convertedPath += '/index.js';
            fileExists = isFile(convertedPath);
        }
        processFile(state, convertedPath, 0, fileExists);
        state.previousKeys = '';
    } else if (key === '\'') {
        if (state.mark !== -1) {
            state.row = state.mark;
        }
    } else if (key === '"') {
        if (state.mark2 !== -1) {
            state.row = state.mark2;
        }
    } else if (key === '#') {
        state.grepQuery = getInVisual(state);
        state.grepIndex = 0;
        state.mode = GREP;
        calcGrepOutput(state);
    } else if (key === '*') {
        state.searchQuery = getInVisual(state);
        state.reverseSearch = false;
        state.mode = SHORTCUTS;
        if (state.searchQuery !== '') {
            state.col += state.searchQuery.length + 1;
            searchForString(state, state.searchQuery);
            state.searching = true;
            centerScreen(state);
        }
    } else if (key === '%') {
        matchIt(state);
    } else if (key === 'y') {
        copyInVisual(state);
        if (state.row >= state.visual.row) {
            state.row = state.visual.row;
        }
        if (state.col >= state.visual.col) {
            state.col = state.visual.col;
        }
        state.mode = SHORTCUTS;
    } else if (key === 'x') {
        deleteInVisual(state);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === 'd') {
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if ((state.previousKeys === 'g' && key === 'c') || key === 'e') {
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
        state.mode = SHORTCUTS;
        createSnapshot(state);
        state.previousKeys = '';
    } else if (key === 'c') {
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = TYPING;
    } else if (key === '=') {
        if (state.row >= state.visual.row) {
            for (let i = state.visual.row; i <= state.row; i += 1) {
                let indentLevel = i - 1 < 0 ? 0 : getIndentLevelFrom(state, i - 1);
                if (state.data[i].trim().startsWith(')')
                    || state.data[i].trim().startsWith('}')
                    || state.data[i].trim().startsWith('</')
                ) {
                    indentLevel = indentLevel - state.indentAmount >= 0 ? indentLevel - state.indentAmount : 0;
                }
                state.data[i] = ' '.repeat(indentLevel) + state.data[i].trim();
            }
        } else if (state.row < state.visual.row) {
            for (let i = state.row; i <= state.visual.row; i += 1) {
                let indentLevel = i - 1 < 0 ? 0 : getIndentLevelFrom(state, i - 1);
                if (state.data[i].trim().startsWith(')')
                    || state.data[i].trim().startsWith('}')
                    || state.data[i].trim().startsWith('</')
                ) {
                    indentLevel = indentLevel - state.indentAmount >= 0 ? indentLevel - state.indentAmount : 0;
                }
                state.data[i] = ' '.repeat(indentLevel) + state.data[i].trim();
            }
        }
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === 'CTRL_F') {
        state.searchQuery = getInVisual(state);
        state.replacing = true;
        state.mode = SEARCH;
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
        state.mode = SHORTCUTS;
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
        state.mode = SHORTCUTS;
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
        state.mode = SHORTCUTS;
    } else if (key === 'V') {
        state.mode = VISUALLINE;
    } else if (key === 'CTRL_V') {
        state.mode = VISUALBLOCK;
    } else if (key === 'o') {
        const tempRow = state.row;
        const tempCol = state.col;
        state.row = state.visual.row;
        state.col = state.visual.col;
        state.visual.row = tempRow;
        state.visual.col = tempCol;
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleVisualKeys
};
