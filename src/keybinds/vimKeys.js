/* eslint-disable import/no-cycle */
import {
    pasteFromClipboardBefore,
    pasteFromClipboardAfter,
    copyToClipboard,
    isWritable,
    renderScreen,
    createSnapshot,
    applySnapshot,
    centerScreen,
    searchForStringNoWrap,
    searchForString,
    searchBackForString,
    isNumeric,
    saveFile,
    trimTrailingWhitespace,
    logCommand,
    refreshFile,
} from '../util/helper.js';
import { sendKeys } from '../util/sendKeys.js';
import {
    up,
    down,
    left,
    right,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    increaseIndentLevel,
    decreaseIndentLevel,
    getCoorForwardWord,
    topOfFile,
    bottomOfFile,
    upHalfScreen,
    downHalfScreen,
    getCoorsInsideCharSame,
    getCoorsInsideCharDiff,
    getCoorsInsideWord,
    removeInsideAreaSameLine,
    copyInsideAreaSameLine,
    getIndentLevelFrom,
    copyInVisual,
    deleteInVisual,
    setVisualHighlight,
    isEmptyRow,
    endOfLine,
    findForward,
    findBackward,
    toForward,
    toBackward,
    isCommented,
    comment,
    uncomment,
    previousSameIndentLevel,
    nextSameIndentLevel,
    previousLowerIndentLevel,
    nextLowerIndentLevel,
    goToCoor,
} from '../util/movement.js';

function handleVimKeys(key, state, screen) {
    state.searching = false;
    if (key === 'ESCAPE') {
        state.mode = 'n';
        state.previousKeys = '';
    } else if (state.previousKeys === '-') {
        if (isNumeric(key)) {
            state.lineNumber += key;
            goToCoor(state, parseInt(state.lineNumber));
        } else if (key === 'ENTER') {
            state.previousKeys = '';
        }
        renderScreen(state, screen);
    } else if (state.previousKeys === 'd') {
        if (key === 'w') {
            const endOfWord = getCoorForwardWord(state);
            copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)], false);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.previousKeys = '';
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 'j') {
            const newClipboard = [];
            newClipboard.push(state.data[state.row]);
            if (state.data[state.row + 1] !== undefined) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.row, 1);
            state.data.splice(state.row, 1);
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            if (state.row < 0) {
                state.row = 0;
            }
            state.col = firstNonSpace(state, state.row);
            state.previousKeys = '';
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 'k') {
            const newClipboard = [];
            if (state.data[state.row - 1] !== undefined) {
                newClipboard.push(state.data[state.row - 1]);
            }
            newClipboard.push(state.data[state.row]);
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.row, 1);
            if (state.data[state.row - 1] !== undefined) {
                state.data.splice(state.row - 1, 1);
            }
            if (state.row < 0) {
                state.row = 0;
            }
            up(state);
            state.col = firstNonSpace(state, state.row);
            state.previousKeys = '';
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T') {
            state.previousKeys += key;
            logCommand(false, state, key);
        } else if (key === 'd') {
            copyToClipboard(state, [state.data[state.row]], true);
            state.data.splice(state.row, 1);
            state.col = firstNonSpace(state, state.row);
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            if (state.row < 0) {
                state.row = 0;
            }
            state.previousKeys = '';
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        }
    } else if (state.previousKeys === 'df') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findForward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'dF') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findBackward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'dt') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = toForward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'dT') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = toBackward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'di') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === '[' || key === ']') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === '\'' || key === '"') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yf') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = findForward(state, key);
            copyInVisual(state);
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yF') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findBackward(state, key);
            copyInVisual(state);
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yt') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = toForward(state, key);
            copyInVisual(state);
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yT') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = toBackward(state, key);
            copyInVisual(state);
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyInsideAreaSameLine(state, beginning, end);
            renderScreen(state, screen);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyInsideAreaSameLine(state, beginning, end);
            renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyInsideAreaSameLine(state, beginning, end);
            renderScreen(state, screen);
        } else if (key === '\'' || key === '"') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyInsideAreaSameLine(state, beginning, end);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'y') {
        if (key === 'j') {
            const newClipboard = [];
            newClipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard, true);
            logCommand(false, state, key);
            state.previousKeys = '';
        } else if (key === 'k') {
            const newClipboard = [];
            if (state.data[state.row - 1]) {
                newClipboard.push(state.data[state.row - 1]);
            }
            newClipboard.push(state.data[state.row]);
            copyToClipboard(state, newClipboard, true);
            logCommand(false, state, key);
            state.previousKeys = '';
        } else if (key === 'y') {
            copyToClipboard(state, [state.data[state.row]], true);
            logCommand(false, state, key);
            state.previousKeys = '';
        } else if (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T') {
            state.previousKeys += key;
        }
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
    } else if (state.previousKeys === 'r') {
        if (isWritable(key)) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + key
                + state.data[state.row].substring(state.col + 1);
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        createSnapshot(state);
        state.previousKeys = '';
    } else if (state.previousKeys.endsWith('g')) {
        if (key === 'g') {
            topOfFile(state);
            renderScreen(state, screen);
        } else if (key === 't') {
            trimTrailingWhitespace(state);
            logCommand(true, state, 'g');
            logCommand(false, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 'c') {
            if (isCommented(state, state.row)) {
                uncomment(state, state.row);
            } else {
                comment(state, state.row);
            }
            createSnapshot(state);
            logCommand(true, state, 'g');
            logCommand(false, state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'c') {
        if (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T') {
            state.previousKeys += key;
            logCommand(false, state, key);
        } else if (key === 'j') {
            const newClipboard = [];
            newClipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.row, 1);
            state.data.splice(state.row, 1);
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            const indentLevel = getIndentLevelFrom(state, state.row);
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.previousKeys = '';
            state.mode = 'i';
            createSnapshot(state);
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === 'k') {
            const newClipboard = [];
            if (state.data[state.row - 1]) {
                newClipboard.push(state.data[state.row - 1]);
            }
            newClipboard.push(state.data[state.row]);
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.row, 1);
            if (state.data[state.row - 1]) {
                state.data.splice(state.row - 1, 1);
            }
            up(state);
            const indentLevel = getIndentLevelFrom(state, state.row);
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.previousKeys = '';
            state.mode = 'i';
            createSnapshot(state);
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === 'w') {
            const endOfWord = getCoorForwardWord(state);
            copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)], false);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.mode = 'i';
            state.previousKeys = '';
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === 'c') {
            let indentLevel = 0;
            if (state.row - 1 > 0) {
                indentLevel = getIndentLevelFrom(state, state.row - 1);
            }
            state.col = indentLevel;
            state.data[state.row] = ' '.repeat(indentLevel);
            state.mode = 'i';
            state.previousKeys = '';
            logCommand(false, state, key);
            renderScreen(state, screen);
        }
    } else if (state.previousKeys === 'cf') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findForward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            state.mode = 'i';
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'cF') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findBackward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            state.mode = 'i';
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ct') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = toForward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            state.mode = 'i';
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'cT') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = toBackward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            state.mode = 'i';
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '[' || key === ']') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '\'' || key === '"') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.mode === 'n') {
        if (key === 'CTRL_U') {
            upHalfScreen(state);
            renderScreen(state, screen);
        } else if (key === 'CTRL_D') {
            downHalfScreen(state);
            renderScreen(state, screen);
        } else if (key === 'UP' || key === 'k') {
            up(state);
            renderScreen(state, screen);
        } else if (key === 'DOWN' || key === 'j') {
            down(state);
            renderScreen(state, screen);
        } else if (key === 'LEFT' || key === 'h') {
            left(state);
            renderScreen(state, screen);
        } else if (key === 'RIGHT' || key === 'l') {
            right(state);
            renderScreen(state, screen);
        } else if (key === 'i') {
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'a') {
            right(state);
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'x') {
            if (state.col < state.data[state.row].length) {
                copyToClipboard(state, [state.data[state.row].substring(state.col, state.col + 1)], false);
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            logCommand(true, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === '$') {
            state.col = endOfLine(state, state.row);
            renderScreen(state, screen);
        } else if (key === '0') {
            state.col = 0;
            renderScreen(state, screen);
        } else if (key === '^') {
            state.col = firstNonSpace(state, state.row);
            renderScreen(state, screen);
        } else if (key === 'I') {
            state.col = firstNonSpace(state, state.row);
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'w') {
            state.col = getCoorBeginningNextWord(state);
            renderScreen(state, screen);
        } else if (key === 'b') {
            state.col = getCoorBeginningLastWord(state);
            renderScreen(state, screen);
        } else if (key === 'A') {
            state.col = state.data[state.row].length;
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'S') {
            state.data[state.row] = '';
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'C') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'D') {
            copyToClipboard(state, [state.data[state.row].substring(state.col)], false);
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            logCommand(true, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 's') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'O') {
            const indentLevel = getIndentLevelFrom(state, state.row, true);
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'o') {
            const indentLevel = getIndentLevelFrom(state, state.row);
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
            state.row += 1;
            state.col = indentLevel;
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === '>') {
            increaseIndentLevel(state, state.row);
            state.col = firstNonSpace(state, state.row);
            logCommand(true, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === '<') {
            decreaseIndentLevel(state, state.row);
            state.col = firstNonSpace(state, state.row);
            logCommand(true, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 'G') {
            bottomOfFile(state);
            renderScreen(state, screen);
        } else if (key === 'p') {
            if (pasteFromClipboardAfter(state)) {
                logCommand(true, state, key);
                createSnapshot(state);
                renderScreen(state, screen);
            }
        } else if (key === 'P') {
            if (pasteFromClipboardBefore(state)) {
                logCommand(true, state, key);
                createSnapshot(state);
                renderScreen(state, screen);
            }
        } else if (key === 'f' || key === 't' || key === 'F' || key === 'T' || key === 'g' || key === '-') {
            if (key === '-') {
                state.lineNumber = '';
            }
            state.previousKeys = key;
        } else if (key === 'c' || key === 'r' || key === 'd' || key === 'y') {
            state.previousKeys = key;
            logCommand(true, state, key);
        } else if (key === 'V') {
            state.mode = 'V';
            state.visualLine = {
                row: state.row
            };
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'n') {
            if (state.searchQuery !== '') {
                state.searching = true;
                state.col += state.searchQuery.length + 1;
                const result = searchForString(state, state.searchQuery);
                if (!result) {
                    state.col -= state.searchQuery.length + 1;
                } else {
                    centerScreen(state);
                    state.windowLineHorizontal = 0;
                    renderScreen(state, screen);
                }
            }
        } else if (key === 'N') {
            if (state.searchQuery !== '') {
                state.searching = true;
                searchBackForString(state, state.searchQuery);
                centerScreen(state);
                state.windowLineHorizontal = 0;
                renderScreen(state, screen);
            }
        } else if (key === '/') {
            state.mode = '/';
            state.searchQuery = '';
            state.search.row = state.row;
            state.search.col = state.col;
            logCommand(true, state, key);
        } else if (key === 'v') {
            state.mode = 'v';
            state.visual = {
                row: state.row,
                col: state.col
            };
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === '}') {
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    state.row = i;
                    break;
                }
            }
            renderScreen(state, screen);
        } else if (key === '{') {
            for (let i = state.row - 1; i >= 0; i -= 1) {
                if (isEmptyRow(state, i)) {
                    state.row = i;
                    break;
                }
            }
            renderScreen(state, screen);
        } else if (key === 'z') {
            centerScreen(state);
            state.windowLineHorizontal = 0;
            renderScreen(state, screen);
        } else if (key === 'J') {
            if (state.data[state.row + 1] !== undefined) {
                state.col = state.data[state.row].length;
                state.data[state.row] += state.data[state.row + 1].trim();
                state.data.splice(state.row + 1, 1);
            }
            logCommand(true, state, key);
            createSnapshot(state);
            renderScreen(state, screen);
        } else if (key === 'CTRL_R') {
            if (state.currentSnapshot + 1 < state.snapshots.length) {
                applySnapshot(state, state.currentSnapshot + 1, false);
            }
            saveFile(state);
            renderScreen(state, screen);
        } else if (key === 'u') {
            if (state.currentSnapshot - 1 >= 0) {
                applySnapshot(state, state.currentSnapshot - 1, true);
            }
            saveFile(state);
            renderScreen(state, screen);
        } else if (key === '[') {
            previousSameIndentLevel(state, state.row);
            renderScreen(state, screen);
        } else if (key === ']') {
            nextSameIndentLevel(state, state.row);
            renderScreen(state, screen);
        } else if (key === '(') {
            previousLowerIndentLevel(state, state.row);
            renderScreen(state, screen);
        } else if (key === ')') {
            nextLowerIndentLevel(state, state.row);
            renderScreen(state, screen);
        } else if (key === '*') {
            const { beginning, end } = getCoorsInsideWord(state);
            setVisualHighlight(state, beginning, end);
            copyInVisual(state);
            state.searchQuery = state.clipboard[0];
            if (state.searchQuery !== '') {
                state.col += state.searchQuery.length + 1;
                searchForString(state, state.searchQuery);
                centerScreen(state);
            }
            renderScreen(state, screen);
        } else if (key === 'q') {
            if (state.recording) {
                state.recording = false;
            } else {
                state.macro = [];
                state.recording = true;
            }
            renderScreen(state, screen);
        } else if (key === '\'') {
            goToCoor(state, state.mark);
            renderScreen(state, screen);
        } else if (key === 'm') {
            state.mark = state.row + 1;
            renderScreen(state, screen);
        } else if (key === '@') {
            if (state.searchQuery !== '') {
                state.col = 0;
                state.row = 0;
                state.allowCommandLogging = false;
                const search = state.searchQuery;
                while (searchForStringNoWrap(state, search)) {
                    sendKeys(state.macro, state, screen);
                }
                state.allowCommandLogging = true;
                renderScreen(state, screen);
            }
        } else if (key === ',') {
            state.allowCommandLogging = false;
            sendKeys(state.macro, state, screen);
            state.allowCommandLogging = true;
            renderScreen(state, screen);
        } else if (key === '.') {
            state.allowCommandLogging = false;
            sendKeys(state.previousCommand, state, screen);
            state.allowCommandLogging = true;
            renderScreen(state, screen);
        } else if (key === '\\') {
            refreshFile(state);
            renderScreen(state, screen);
        }
    }
}

export {
    handleVimKeys
};
