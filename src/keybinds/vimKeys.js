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
    changeFile,
} from '../util/helper.js';
import { sendKeys } from '../util/sendKeys.js';
import {
    up,
    down,
    left,
    right,
    firstNonSpace,
    getInsideOfIndentLevel,
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
    getIndentLevel,
    getInVisual,
    copyInVisual,
    deleteInVisual,
    setVisualHighlight,
    isEmptyRow,
    endOfLine,
    findForward,
    findBackward,
    toForward,
    toBackward,
    toggleComment,
    uncommentBlock,
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
            goToCoor(state, parseInt(state.lineNumber) - 1);
        } else if (key === 'ENTER') {
            state.previousKeys = '';
        }
    } else if (state.previousKeys === 'd') {
        if (key === 'w') {
            const endOfWord = getCoorForwardWord(state);
            copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)]);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.previousKeys = '';
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'j') {
            const newClipboard = ['\n'];
            newClipboard.push(state.data[state.row]);
            if (state.data[state.row + 1] !== undefined) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard);
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
        } else if (key === 'k') {
            const newClipboard = ['\n'];
            if (state.data[state.row - 1] !== undefined) {
                newClipboard.push(state.data[state.row - 1]);
            }
            newClipboard.push(state.data[state.row]);
            copyToClipboard(state, newClipboard);
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
        } else if (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a') {
            state.previousKeys += key;
            logCommand(false, state, key);
        } else if (key === 'd') {
            copyToClipboard(state, ['\n', state.data[state.row]]);
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
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'da') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
            removeInsideAreaSameLine(state, beginning, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
            removeInsideAreaSameLine(state, beginning, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'p') {
            state.visualLine = {
                row: state.row,
                col: state.col
            };
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
                    state.visualLine = { row: i, col: 0 };
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    state.row = i;
                    break;
                } else {
                    state.row = i;
                    state.col = 0;
                }
            }
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            if (state.row < 0) {
                state.row = 0;
            }
            firstNonSpace(state, state.row);
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning - 1;
                state.row = end + 1;
                state.col = 0;
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
                if (state.row > state.data.length - 1) {
                    state.row = state.data.length - 1;
                }
                if (state.row < 0) {
                    state.row = 0;
                }
                firstNonSpace(state, state.row);
                createSnapshot(state);
            }
            logCommand(false, state, key);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'di') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'p') {
            state.visualLine = {
                row: state.row,
                col: state.col
            };
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
                    state.visualLine = { row: i, col: 0 };
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.row = i;
                    state.col = 0;
                }
            }
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            if (state.row < 0) {
                state.row = 0;
            }
            firstNonSpace(state, state.row);
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning;
                state.row = end;
                state.col = 0;
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
                if (state.row > state.data.length - 1) {
                    state.row = state.data.length - 1;
                }
                if (state.row < 0) {
                    state.row = 0;
                }
                firstNonSpace(state, state.row);
                createSnapshot(state);
            }
            logCommand(false, state, key);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'n');
            logCommand(false, state, key);
            createSnapshot(state);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yf') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = findForward(state, key);
            copyInVisual(state);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yF') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findBackward(state, key);
            copyInVisual(state);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yt') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = toForward(state, key);
            copyInVisual(state);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yT') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = toBackward(state, key);
            copyInVisual(state);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'ya') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === 'p') {
            state.visualLine = {
                row: state.row,
                col: state.col
            };
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
                    state.visualLine = { row: i, col: 0 };
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.row = i;
                    state.col = 0;
                }
            }
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
            }
            createSnapshot(state);
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning - 1;
                state.row = end + 1;
                state.col = 0;
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.row = state.visualLine.row;
                firstNonSpace(state, state.row);
            }
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyInsideAreaSameLine(state, beginning, end);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === 'p') {
            state.visualLine = {
                row: state.row,
                col: state.col
            };
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
                    state.visualLine = { row: i, col: 0 };
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.row = i;
                    state.col = 0;
                }
            }
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
            }
            createSnapshot(state);
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning;
                state.row = end;
                state.col = 0;
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.row = state.visualLine.row;
                firstNonSpace(state, state.row);
            }
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyInsideAreaSameLine(state, beginning, end);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyInsideAreaSameLine(state, beginning, end);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'y') {
        if (key === 'j') {
            const newClipboard = ['\n'];
            newClipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard);
            state.previousKeys = '';
        } else if (key === 'k') {
            const newClipboard = ['\n'];
            if (state.data[state.row - 1]) {
                newClipboard.push(state.data[state.row - 1]);
            }
            newClipboard.push(state.data[state.row]);
            copyToClipboard(state, newClipboard);
            state.previousKeys = '';
        } else if (key === 'y') {
            copyToClipboard(state, ['\n', state.data[state.row]]);
            state.previousKeys = '';
        } else if (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a') {
            state.previousKeys += key;
        }
    } else if (state.previousKeys === 'T') {
        if (isWritable(key)) {
            state.col = toBackward(state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 't') {
        if (isWritable(key)) {
            state.col = toForward(state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'F') {
        if (isWritable(key)) {
            state.col = findBackward(state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'f') {
        if (isWritable(key)) {
            state.col = findForward(state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'r') {
        if (isWritable(key)) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + key
                + state.data[state.row].substring(state.col + 1);
        }
        logCommand(false, state, key);
        createSnapshot(state);
        state.previousKeys = '';
    } else if (state.previousKeys.endsWith('g')) {
        if (key === 'g') {
            topOfFile(state);
        } else if (key === 't') {
            trimTrailingWhitespace(state);
            logCommand(true, state, 'g');
            logCommand(false, state, key);
            createSnapshot(state);
        } else if (key === 'e') {
            uncommentBlock(state, state.row);
            createSnapshot(state);
            logCommand(true, state, 'g');
            logCommand(false, state, key);
        } else if (key === 'c') {
            toggleComment(state, state.row);
            createSnapshot(state);
            logCommand(true, state, 'g');
            logCommand(false, state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'c') {
        if (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a') {
            state.previousKeys += key;
            logCommand(false, state, key);
        } else if (key === 'j') {
            const newClipboard = ['\n'];
            newClipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard);
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
        } else if (key === 'k') {
            const newClipboard = ['\n'];
            if (state.data[state.row - 1]) {
                newClipboard.push(state.data[state.row - 1]);
            }
            newClipboard.push(state.data[state.row]);
            copyToClipboard(state, newClipboard);
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
        } else if (key === 'w') {
            const endOfWord = getCoorForwardWord(state);
            copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)]);
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.mode = 'i';
            state.previousKeys = '';
            logCommand(false, state, key);
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
        }
    } else if (state.previousKeys === 'cf') {
        if (isWritable(key)) {
            state.visual.row = state.row;
            state.visual.col = state.col;
            state.col = findForward(state, key);
            copyInVisual(state);
            deleteInVisual(state);
            state.mode = 'i';
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
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ca') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
            removeInsideAreaSameLine(state, beginning, end + 1, 'i');
            logCommand(false, state, key);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
            removeInsideAreaSameLine(state, beginning, end + 1, 'i');
            logCommand(false, state, key);
        } else if (key === 'p') {
            state.visualLine = {
                row: state.row,
                col: state.col
            };
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
                    state.visualLine = { row: i, col: 0 };
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    state.row = i;
                    break;
                } else {
                    state.row = i;
                    state.col = 0;
                }
            }
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row) : 0;
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            logCommand(false, state, key);
            state.mode = 'i';
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning - 1;
                state.row = end + 1;
                state.col = 0;
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
                const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row - 1) : 0;
                state.data.splice(state.row, 0, ' '.repeat(indentLevel));
                state.col = indentLevel;
                state.mode = 'i';
            }
            logCommand(false, state, key);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
            logCommand(false, state, key);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
            logCommand(false, state, key);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
            logCommand(false, state, key);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
            logCommand(false, state, key);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
            removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
            logCommand(false, state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        } else if (key === 'W') {
            const { beginning, end } = getCoorsInsideCharSame(state, ' ');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        } else if (key === 'p') {
            state.visualLine = {
                row: state.row,
                col: state.col
            };
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
                    state.visualLine = { row: i, col: 0 };
                }
            }
            for (let i = state.row + 1; i < state.data.length; i += 1) {
                if (isEmptyRow(state, i)) {
                    break;
                } else {
                    state.row = i;
                    state.col = 0;
                }
            }
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row) : 0;
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            logCommand(false, state, key);
            state.mode = 'i';
        } else if (key === 'f') {
            if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
                const { beginning, end } = getInsideOfIndentLevel(state);
                state.visualLine.row = beginning;
                state.row = end;
                state.col = 0;
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
                const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row - 1) : 0;
                state.data.splice(state.row, 0, ' '.repeat(indentLevel));
                state.col = indentLevel;
                state.mode = 'i';
            }
            logCommand(false, state, key);
        } else if (key === '[' || key === ']' || key === 'd') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        } else if (key === '<' || key === '>' || key === 't') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        } else if (key === '\'' || key === '"' || key === '`') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
            removeInsideAreaSameLine(state, beginning, end, 'i');
            logCommand(false, state, key);
        }
        state.previousKeys = '';
    } else if (state.mode === 'n') {
        if (key === 'CTRL_O') {
            if (state.fileIndex - 1 >= 0) {
                state.file = state.files[state.fileIndex - 1];
                const snapshotsCopy = [];
                for (let i = 0; i < state.snapshots.length; i += 1) {
                    snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                }
                state.storePosition[state.fileIndex] = {
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
                };
                state.fileIndex -= 1;
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
        } else if (key === 'TAB') {
            if (state.fileIndex + 1 < state.files.length) {
                state.file = state.files[state.fileIndex + 1];
                const snapshotsCopy = [];
                for (let i = 0; i < state.snapshots.length; i += 1) {
                    snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                }
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
                state.fileIndex += 1;
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
        } else if (key === 'CTRL_F') {
            state.replacing = true;
            state.mode = '/';
            logCommand(true, state, key);
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
        } else if (key === 'i') {
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'a') {
            right(state);
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'x') {
            if (state.col < state.data[state.row].length) {
                copyToClipboard(state, [state.data[state.row].substring(state.col, state.col + 1)]);
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            logCommand(true, state, key);
            createSnapshot(state);
        } else if (key === '$') {
            state.col = endOfLine(state, state.row);
        } else if (key === '1'
            || key === '2'
            || key === '3'
            || key === '4'
            || key === '5'
            || key === '6'
            || key === '7'
            || key === '8'
            || key === '9'
        ) {
            state.previousKeys = '-';
            state.lineNumber = '';
            state.lineNumber += key;
        } else if (key === '0') {
            state.col = 0;
        } else if (key === '^') {
            state.col = firstNonSpace(state, state.row);
        } else if (key === 'I') {
            state.col = firstNonSpace(state, state.row);
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'w') {
            state.col = getCoorBeginningNextWord(state);
        } else if (key === 'b') {
            state.col = getCoorBeginningLastWord(state);
        } else if (key === 'A') {
            state.col = state.data[state.row].length;
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'S') {
            state.data[state.row] = '';
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'Y') {
            copyToClipboard(state, [state.data[state.row].substring(state.col)]);
        } else if (key === 'C') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'D') {
            copyToClipboard(state, [state.data[state.row].substring(state.col)]);
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            logCommand(true, state, key);
            createSnapshot(state);
        } else if (key === 's') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'O') {
            const indentLevel = getIndentLevelFrom(state, state.row, true);
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'o') {
            const indentLevel = getIndentLevelFrom(state, state.row);
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
            down(state);
            state.col = indentLevel;
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === '>') {
            increaseIndentLevel(state, state.row);
            state.col = firstNonSpace(state, state.row);
            logCommand(true, state, key);
            createSnapshot(state);
        } else if (key === '<') {
            decreaseIndentLevel(state, state.row);
            state.col = firstNonSpace(state, state.row);
            logCommand(true, state, key);
            createSnapshot(state);
        } else if (key === 'G') {
            bottomOfFile(state);
        } else if (key === 'p') {
            if (pasteFromClipboardAfter(state)) {
                logCommand(true, state, key);
                createSnapshot(state);
            }
        } else if (key === 'P') {
            if (pasteFromClipboardBefore(state)) {
                logCommand(true, state, key);
                createSnapshot(state);
            }
        } else if (key === 'f' || key === 't' || key === 'F' || key === 'T' || key === 'g' || key === '-' || key === 'y') {
            if (key === '-') {
                state.lineNumber = '';
            }
            state.previousKeys = key;
        } else if (key === 'c' || key === 'r' || key === 'd') {
            state.previousKeys = key;
            logCommand(true, state, key);
        } else if (key === 'CTRL_V') {
            state.mode = 'CTRL_V';
            state.visualBlock = {
                row: state.row,
                col: state.col
            };
            logCommand(true, state, key);
        } else if (key === 'V') {
            state.mode = 'V';
            state.visualLine = {
                row: state.row
            };
            logCommand(true, state, key);
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
                }
            }
        } else if (key === 'N') {
            if (state.searchQuery !== '') {
                state.searching = true;
                searchBackForString(state, state.searchQuery);
                centerScreen(state);
                state.windowLineHorizontal = 0;
            }
        } else if (key === '/') {
            state.mode = '/';
            state.searchQuery = '';
            state.search.row = state.row;
            state.search.col = state.col;
        } else if (key === 'v') {
            state.mode = 'v';
            state.visual = {
                row: state.row,
                col: state.col
            };
            logCommand(true, state, key);
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
        } else if (key === 'z') {
            centerScreen(state);
            state.windowLineHorizontal = 0;
            renderScreen(state, screen, undefined, true);
        } else if (key === 'K') {
            if (state.data[state.row + 1] !== undefined) {
                state.col = state.data[state.row].length;
                state.data[state.row] += ' ' + state.data[state.row + 1].trim();
                state.data.splice(state.row + 1, 1);
            }
            logCommand(true, state, key);
            createSnapshot(state);
        } else if (key === 'J') {
            if (state.data[state.row + 1] !== undefined) {
                state.col = state.data[state.row].length;
                state.data[state.row] += state.data[state.row + 1].trim();
                state.data.splice(state.row + 1, 1);
            }
            logCommand(true, state, key);
            createSnapshot(state);
        } else if (key === 'CTRL_R') {
            if (state.currentSnapshot + 1 < state.snapshots.length) {
                applySnapshot(state, state.currentSnapshot + 1, false);
            }
            saveFile(state);
        } else if (key === 'u') {
            if (state.currentSnapshot - 1 >= 0) {
                applySnapshot(state, state.currentSnapshot - 1, true);
            }
            saveFile(state);
        } else if (key === '[') {
            previousSameIndentLevel(state, state.row);
        } else if (key === ']') {
            nextSameIndentLevel(state, state.row);
        } else if (key === '(') {
            previousLowerIndentLevel(state, state.row);
        } else if (key === ')') {
            nextLowerIndentLevel(state, state.row);
        } else if (key === '*') {
            const { beginning, end } = getCoorsInsideWord(state);
            setVisualHighlight(state, beginning, end);
            state.searchQuery = getInVisual(state);
            if (state.searchQuery !== '') {
                state.col += state.searchQuery.length + 1;
                searchForString(state, state.searchQuery);
                state.searching = true;
                centerScreen(state);
            }
        } else if (key === 'q') {
            if (state.recording) {
                state.recording = false;
            } else {
                state.macro = [];
                state.recording = true;
            }
        } else if (key === '=') {
            let indentLevel = state.row - 1 < 0 ? 0 : getIndentLevelFrom(state, state.row - 1);
            if (state.data[state.row].trim().startsWith(')') || state.data[state.row].trim().startsWith('}')) {
                indentLevel = indentLevel - 4 >= 0 ? indentLevel - 4 : 0;
            }
            state.data[state.row] = ' '.repeat(indentLevel) + state.data[state.row].trim();
            createSnapshot(state);
            goToCoor(state, state.mark);
        } else if (key === 'CTRL_P') {
            state.mode = 'f';
        } else if (key === '\'') {
            state.row = state.mark;
        } else if (key === 'm') {
            state.mark = state.row;
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
            }
        } else if (key === ',') {
            state.allowCommandLogging = false;
            sendKeys(state.macro, state, screen);
            state.allowCommandLogging = true;
        } else if (key === '.') {
            state.allowCommandLogging = false;
            sendKeys(state.previousCommand, state, screen);
            state.allowCommandLogging = true;
        } else if (key === '\\') {
            refreshFile(state);
        }
    }
    renderScreen(state, screen);
}

export {
    handleVimKeys
};
