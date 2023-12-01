/* eslint-disable import/no-cycle */
import {
    copyToClipboard,
    createSnapshot,
    getFormattedLines,
    getSystemPaste,
    logCommand,
    pasteFromClipboardBefore,
    renderScreen,
    sortLines,
} from '../util/helper.js';
import { sendKeys } from '../util/sendKeys.js';
import {
    SHORTCUTS,
    TYPING,
    VISUAL,
    VISUALBLOCK,
} from '../util/modes.js';
import {
    bottomOfFile,
    commentVisualLines,
    decreaseIndentLevel,
    down,
    downHalfScreen,
    endOfLine,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    getIndentLevelFrom,
    increaseIndentLevel,
    isEmptyRow,
    left,
    matchIt,
    nextLowerIndentLevel,
    nextSameIndentLevel,
    previousLowerIndentLevel,
    previousSameIndentLevel,
    right,
    topOfFile,
    up,
    upHalfScreen,
} from '../util/movement.js';

function handleVisualLineKeys(key, state, screen) {
    if (key === 'CTRL_U') {
        upHalfScreen(state);
    } else if (key === 'CTRL_D') {
        downHalfScreen(state);
    } else if (key === '\'') {
        if (state.mark !== -1) {
            state.row = state.mark;
        }
    } else if (key === '"') {
        if (state.mark2 !== -1) {
            state.row = state.mark2;
        }
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
    } else if (key === 'y') {
        if (state.row >= state.visual.row) {
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.row = state.visual.row;
        } else if (state.row < state.visual.row) {
            const newClipboard = [''];
            for (let i = state.row; i <= state.visual.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
        }
        state.mode = SHORTCUTS;
    } else if (key === 'e') {
        commentVisualLines(state, Math.min(state.row, state.visual.row), Math.max(state.row, state.visual.row));
        state.mode = SHORTCUTS;
        createSnapshot(state);
        state.previousKeys = '';
    } else if (key === 'c') {
        if (state.row >= state.visual.row) {
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
        } else if (state.row < state.visual.row) {
            const newClipboard = [''];
            for (let i = state.row; i <= state.visual.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.row, state.visual.row - state.row + 1);
        }
        const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row) : 0;
        state.data.splice(state.row, 0, ' '.repeat(indentLevel));
        state.col = indentLevel;
        state.mode = TYPING;
    } else if (key === 'd') {
        if (state.row >= state.visual.row) {
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
        } else if (state.row < state.visual.row) {
            const newClipboard = [''];
            for (let i = state.row; i <= state.visual.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.row, state.visual.row - state.row + 1);
        }
        if (state.row > state.data.length - 1) {
            state.row = state.data.length - 1;
        }
        if (state.row < 0) {
            state.row = 0;
        }
        firstNonSpace(state, state.row);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === 'g') {
        if (state.previousKeys === 'g') {
            topOfFile(state);
            state.previousKeys = '';
        } else {
            state.previousKeys = 'g';
        }
    } else if (key === 'G') {
        bottomOfFile(state);
    } else if (key === ',') {
        state.mode = SHORTCUTS;
        state.allowCommandLogging = false;
        const start = Math.min(state.visual.row, state.row);
        const end = Math.max(state.visual.row, state.row);
        state.row = start;
        state.visual.row = end;
        for (let i = state.row; i <= state.visual.row; i += 1) {
            sendKeys(state.macro, state, screen);
            down(state);
        }
        state.allowCommandLogging = true;
        createSnapshot(state);
        renderScreen(state, screen);
    } else if (key === '_') {
        const sortedLines = sortLines(state);
        let begin = state.row;
        let end = state.visual.row;
        if (begin > end) {
            begin = state.visual.row;
            end = state.row;
        }
        const args = [begin, end - begin + 1].concat(sortedLines);
        Array.prototype.splice.apply(state.data, args);
        createSnapshot(state);
        state.mode = SHORTCUTS;
    } else if (key === '}') {
        for (let i = state.row + 1; i < state.data.length; i += 1) {
            if (isEmptyRow(state, i)) {
                state.row = i;
                break;
            }
        }
    } else if (key === '{') {
        for (let i = state.row - 1; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                state.row = i;
                break;
            }
        }
    } else if (key === 'J') {
        const start = Math.min(state.row, state.visual.row);
        const end = Math.max(state.row, state.visual.row);
        if (end < state.data.length - 1) {
            const temp = state.data[end + 1];
            state.data.splice(end + 1, 1);
            state.data.splice(start, 0, temp);
            state.row += 1;
            state.visual.row += 1;
            const lines = getFormattedLines(state, start, end);
            for (let i = 0; i <= end - start; i += 1) {
                state.data[start + i] = lines[i];
            }
        }
        createSnapshot(state);
    } else if (key === 'K') {
        const start = Math.min(state.row, state.visual.row);
        const end = Math.max(state.row, state.visual.row);
        if (start > 0) {
            const temp = state.data[start - 1];
            state.data.splice(start - 1, 1);
            state.data.splice(end, 0, temp);
            state.row -= 1;
            state.visual.row -= 1;
            const lines = getFormattedLines(state, start, end);
            for (let i = 0; i <= end - start; i += 1) {
                state.data[start + i] = lines[i];
            }
        }
        createSnapshot(state);
    } else if (key === '=') {
        const start = Math.min(state.row, state.visual.row);
        const end = Math.max(state.row, state.visual.row);
        const lines = getFormattedLines(state, start, end);
        for (let i = 0; i <= end - start; i += 1) {
            state.data[start + i] = lines[i];
        }
        state.row = start;
        state.col = firstNonSpace(state, state.row);
        createSnapshot(state);
        state.mode = SHORTCUTS;
    } else if (key === '<') {
        const start = Math.min(state.row, state.visual.row);
        const end = Math.max(state.row, state.visual.row);
        for (let i = start; i <= end; i += 1) {
            decreaseIndentLevel(state, i);
        }
        createSnapshot(state);
        if (state.row === start) {
            state.row = end;
        } else {
            state.row = start;
        }
        state.mode = SHORTCUTS;
    } else if (key === '>') {
        const start = Math.min(state.row, state.visual.row);
        const end = Math.max(state.row, state.visual.row);
        for (let i = start; i <= end; i += 1) {
            increaseIndentLevel(state, i);
        }
        createSnapshot(state);
        if (state.row === start) {
            state.row = end;
        } else {
            state.row = start;
        }
        state.mode = SHORTCUTS;
    } else if (key === 'x') {
        if (state.row >= state.visual.row) {
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
        } else if (state.row < state.visual.row) {
            state.data.splice(state.row, state.visual.row - state.row + 1);
        }
        if (state.row > state.data.length - 1) {
            state.row = state.data.length - 1;
        }
        if (state.row < 0) {
            state.row = 0;
        }
        firstNonSpace(state, state.row);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === 'p' || key === 'P') {
        let systemPaste = getSystemPaste(state);
        if (!systemPaste.startsWith('\n')) {
            systemPaste = '\n' + systemPaste;
        }
        systemPaste = systemPaste.split('\n');
        if (systemPaste.length > 0) {
            if (state.row >= state.visual.row) {
                state.data.splice(state.visual.row, state.row - state.visual.row + 1);
                state.row = state.visual.row;
            } else if (state.row < state.visual.row) {
                state.data.splice(state.row, state.visual.row - state.row + 1);
            }
            if (state.row >= state.data.length) {
                state.data.splice(state.row, 0, '');
                pasteFromClipboardBefore(state);
            } else {
                pasteFromClipboardBefore(state);
            }
        }
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === 'i') {
        state.previousKeys = 'i';
    } else if (key === '[') {
        previousSameIndentLevel(state, state.row);
        renderScreen(state, screen);
    } else if (key === ']') {
        nextSameIndentLevel(state, state.row);
        renderScreen(state, screen);
    } else if (key === '%') {
        matchIt(state);
    } else if (key === '(') {
        previousLowerIndentLevel(state, state.row);
        renderScreen(state, screen);
    } else if (key === ')') {
        nextLowerIndentLevel(state, state.row);
        renderScreen(state, screen);
    } else if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
    } else if (key === 'v') {
        state.mode = VISUAL;
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
    handleVisualLineKeys
};
