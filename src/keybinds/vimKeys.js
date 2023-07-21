/* eslint-disable import/no-cycle */
import moment from 'moment';
import { execSync } from 'child_process';
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
    processFile,
    logCommand,
    refreshFile,
    getSystemPaste,
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
    swapLeft,
    swapRight,
    matchIt,
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
        } else {
            state.previousKeys = '';
            sendKeys([key], state, screen);
        }
    } else if (state.previousKeys + key === 'dw') {
        const endOfWord = getCoorForwardWord(state);
        copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)]);
        state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'dj') {
        const newClipboard = [''];
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
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'dk') {
        const newClipboard = [''];
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
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'd' && (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a')) {
        state.previousKeys += key;
        logCommand(false, state, key);
    } else if (state.previousKeys + key === 'dd') {
        copyToClipboard(state, ['', state.data[state.row]]);
        state.data.splice(state.row, 1);
        state.col = firstNonSpace(state, state.row);
        if (state.row > state.data.length - 1) {
            state.row = state.data.length - 1;
        }
        if (state.row < 0) {
            state.row = 0;
        }
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'df' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'dF' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'dt' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'dT' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'daw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'daW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'dap') {
        // TODO simplify
        state.visual = {
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
        state.visual.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visual = { row: i, col: 0 };
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
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'daf') {
        // TODO simplify
        if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
            const { beginning, end } = getInsideOfIndentLevel(state);
            state.visual.row = beginning - 1;
            state.row = end + 1;
            state.col = 0;
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            if (state.row < 0) {
                state.row = 0;
            }
            firstNonSpace(state, state.row);
        }
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'da' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'da' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'da' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'da' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'da' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'diw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'diW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'dip') {
        // TODO simplify
        state.visual = {
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
        state.visual.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visual = { row: i, col: 0 };
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
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'dif') {
        // TODO simplify
        if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
            const { beginning, end } = getInsideOfIndentLevel(state);
            state.visual.row = beginning;
            state.row = end;
            state.col = 0;
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            if (state.row < 0) {
                state.row = 0;
            }
            firstNonSpace(state, state.row);
        }
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'di' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'di' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'di' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'di' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'di' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'n');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'yf' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = findForward(state, key);
        copyInVisual(state);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yF' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findBackward(state, key);
        copyInVisual(state);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yt' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = toForward(state, key);
        copyInVisual(state);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yT' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toBackward(state, key);
        copyInVisual(state);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yaw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yaW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yap') {
        state.visual = {
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
        state.visual.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visual = { row: i, col: 0 };
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
        if (state.row > state.data.length - 1) {
            state.row = state.data.length - 1;
        }
        if (state.row < 0) {
            state.row = 0;
        }
        firstNonSpace(state, state.row);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yaf') {
        if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
            const { beginning, end } = getInsideOfIndentLevel(state);
            state.visual.row = beginning - 1;
            state.row = end + 1;
            state.col = 0;
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.row = state.visual.row;
            firstNonSpace(state, state.row);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'ya' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ya' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ya' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ya' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ya' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yiw') {
        state.previousKeys = 'hello';
        const { beginning, end } = getCoorsInsideWord(state);
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yiW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yip') {
        state.visual = {
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
        state.visual.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visual = { row: i, col: 0 };
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
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yif') {
        if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
            const { beginning, end } = getInsideOfIndentLevel(state);
            state.visual.row = beginning;
            state.row = end;
            state.col = 0;
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.row = state.visual.row;
            firstNonSpace(state, state.row);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyInsideAreaSameLine(state, beginning, end);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yj') {
        const newClipboard = [''];
        newClipboard.push(state.data[state.row]);
        if (state.data[state.row + 1]) {
            newClipboard.push(state.data[state.row + 1]);
        }
        copyToClipboard(state, newClipboard);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yk') {
        const newClipboard = [''];
        if (state.data[state.row - 1]) {
            newClipboard.push(state.data[state.row - 1]);
        }
        newClipboard.push(state.data[state.row]);
        copyToClipboard(state, newClipboard);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'yy') {
        copyToClipboard(state, ['', state.data[state.row]]);
        state.previousKeys = '';
    } else if (state.previousKeys === 'y' && (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a')) {
        state.previousKeys += key;
    } else if (state.previousKeys === 'T' && isWritable(key)) {
        state.col = toBackward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['T', key];
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 't' && isWritable(key)) {
        state.col = toForward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['t', key];
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'F' && isWritable(key)) {
        state.col = findBackward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['F', key];
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'f' && isWritable(key)) {
        state.col = findForward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['f', key];
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'r' && isWritable(key)) {
        state.data[state.row] = state.data[state.row].substring(0, state.col) + key + state.data[state.row].substring(state.col + 1);
        logCommand(false, state, key);
        createSnapshot(state);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'gg') {
        topOfFile(state);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'gt') {
        trimTrailingWhitespace(state);
        logCommand(true, state, 'g');
        logCommand(false, state, key);
        createSnapshot(state);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'ge') {
        uncommentBlock(state, state.row);
        logCommand(true, state, 'g');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys + key === 'gc') {
        toggleComment(state, state.row);
        logCommand(true, state, 'g');
        logCommand(false, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === 'c' && (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a')) {
        state.previousKeys += key;
        logCommand(false, state, key);
    } else if (state.previousKeys + key === 'cj') {
        // TODO cleanup c<key>
        const newClipboard = [''];
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
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'ck') {
        const newClipboard = [''];
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
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'cw') {
        const endOfWord = getCoorForwardWord(state);
        copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)]);
        state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'cc') {
        let indentLevel = 0;
        if (state.row - 1 > 0) {
            indentLevel = getIndentLevelFrom(state, state.row - 1);
        }
        state.col = indentLevel;
        state.data[state.row] = ' '.repeat(indentLevel);
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'cf' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'cF' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ct' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'cT' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'i';
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'caw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'caW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'cap') {
        state.visual = {
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
        state.visual.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visual = { row: i, col: 0 };
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
        logCommand(false, state, key);
        state.previousKeys = '';
        state.mode = 'i';
    } else if (state.previousKeys + key === 'caf') {
        if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
            const { beginning, end } = getInsideOfIndentLevel(state);
            state.visual.row = beginning - 1;
            state.row = end + 1;
            state.col = 0;
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
            const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row - 1) : 0;
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ca' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ca' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ca' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ca' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ca' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'ciw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'ciW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys + key === 'cip') {
        state.visual = {
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
        state.visual.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visual = { row: i, col: 0 };
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
        logCommand(false, state, key);
        state.previousKeys = '';
        state.mode = 'i';
    } else if (state.previousKeys + key === 'cif') {
        if (!isEmptyRow(state, state.row) && getIndentLevel(state, state.row) !== 0) {
            const { beginning, end } = getInsideOfIndentLevel(state);
            state.visual.row = beginning;
            state.row = end;
            state.col = 0;
            const newClipboard = [''];
            for (let i = state.visual.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard);
            state.data.splice(state.visual.row, state.row - state.visual.row + 1);
            state.row = state.visual.row;
            const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row - 1) : 0;
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '<', '>');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'ci' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, 'i');
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === '' && key === 'TAB') {
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
            state.mark = pos.mark;
            state.prevRow = pos.prevRow;
            state.prevCol = pos.prevCol;
        }
    } else if (state.previousKeys === '' && key === 'CTRL_O') {
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
            state.mark = pos.mark;
            state.prevRow = pos.prevRow;
            state.prevCol = pos.prevCol;
        }
    } else if (state.previousKeys === '' && key === 'CTRL_W') {
        if (state.harpoonIndex - 1 >= 0) {
            state.harpoonIndex -= 1;
        }
        processFile(state, state.files[state.harpoonIndexes[state.harpoonIndex]], 0, state.files[state.harpoonIndexes[state.harpoonIndex]] !== undefined);
    } else if (state.previousKeys === '' && key === 'CTRL_E') {
        if (state.harpoonIndex + 1 < state.harpoonIndexes.length) {
            state.harpoonIndex += 1;
        }
        processFile(state, state.files[state.harpoonIndexes[state.harpoonIndex]], 0, state.files[state.harpoonIndexes[state.harpoonIndex]] !== undefined);
    } else if (state.previousKeys === '' && key === 'CTRL_X') {
        state.harpoonIndexes = state.harpoonIndexes.filter(((e) => (e !== state.fileIndex)));
        if (state.harpoonIndex - 1 >= 0) {
            state.harpoonIndex -= 1;
        }
        processFile(state, state.files[state.harpoonIndexes[state.harpoonIndex]], 0, state.files[state.harpoonIndexes[state.harpoonIndex]] !== undefined);
    } else if (state.previousKeys === '' && key === 'CTRL_F') {
        state.replacing = true;
        state.mode = '/';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'BACKSPACE') {
        state.harpoonIndex = swapLeft({ array: state.harpoonIndexes, index: state.harpoonIndex });
    } else if (state.previousKeys === '' && key === 'CTRL_L') {
        state.harpoonIndex = swapRight({ array: state.harpoonIndexes, index: state.harpoonIndex });
    } else if (state.previousKeys === '' && key === 'CTRL_U') {
        upHalfScreen(state);
    } else if (state.previousKeys === '' && key === 'CTRL_D') {
        downHalfScreen(state);
    } else if (state.previousKeys === '' && (key === 'UP' || key === 'k')) {
        up(state);
    } else if (state.previousKeys === '' && (key === 'DOWN' || key === 'j')) {
        down(state);
    } else if (state.previousKeys === '' && (key === 'LEFT' || key === 'h')) {
        left(state);
    } else if (state.previousKeys === '' && (key === 'RIGHT' || key === 'l')) {
        right(state);
    } else if (state.previousKeys === '' && key === 'i') {
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'a') {
        right(state);
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'x') {
        if (state.col < state.data[state.row].length) {
            copyToClipboard(state, [state.data[state.row].substring(state.col, state.col + 1)]);
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + state.data[state.row].substring(state.col + 1);
        }
        logCommand(true, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === '$') {
        state.col = endOfLine(state, state.row);
    } else if (
        state.previousKeys === ''
        && (key === '1' || key === '2' || key === '3' || key === '4' || key === '5' || key === '6' || key === '7' || key === '8' || key === '9')
    ) {
        state.previousKeys = '-';
        state.lineNumber = '';
        state.lineNumber += key;
    } else if (state.previousKeys === '' && key === '0') {
        state.col = 0;
    } else if (state.previousKeys === '' && key === '^') {
        state.col = firstNonSpace(state, state.row);
    } else if (state.previousKeys === '' && key === 'I') {
        state.col = firstNonSpace(state, state.row);
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'w') {
        state.col = getCoorBeginningNextWord(state);
    } else if (state.previousKeys === '' && key === 'b') {
        state.col = getCoorBeginningLastWord(state);
    } else if (state.previousKeys === '' && key === 'A') {
        state.col = state.data[state.row].length;
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'S') {
        state.data[state.row] = '';
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'Y') {
        copyToClipboard(state, [state.data[state.row].substring(state.col)]);
    } else if (state.previousKeys === '' && key === 'C') {
        state.data[state.row] = state.data[state.row].substring(0, state.col);
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'D') {
        copyToClipboard(state, [state.data[state.row].substring(state.col)]);
        state.data[state.row] = state.data[state.row].substring(0, state.col);
        logCommand(true, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === 's') {
        if (state.col < state.data[state.row].length) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + state.data[state.row].substring(state.col + 1);
        }
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'O') {
        const indentLevel = getIndentLevelFrom(state, state.row, true);
        state.data.splice(state.row, 0, ' '.repeat(indentLevel));
        state.col = indentLevel;
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'o') {
        const indentLevel = getIndentLevelFrom(state, state.row);
        state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
        down(state);
        state.col = indentLevel;
        state.mode = 'i';
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === '>') {
        increaseIndentLevel(state, state.row);
        state.col = firstNonSpace(state, state.row);
        logCommand(true, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === '<') {
        decreaseIndentLevel(state, state.row);
        state.col = firstNonSpace(state, state.row);
        logCommand(true, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === 'G') {
        bottomOfFile(state);
    } else if (state.previousKeys === '' && key === 'p') {
        if (pasteFromClipboardAfter(state)) {
            logCommand(true, state, key);
            createSnapshot(state);
        }
    } else if (state.previousKeys === '' && key === 'P') {
        if (pasteFromClipboardBefore(state)) {
            logCommand(true, state, key);
            createSnapshot(state);
        }
    } else if (state.previousKeys === '' && (key === 'f' || key === 't' || key === 'F' || key === 'T' || key === 'g' || key === '-' || key === 'y')) {
        if (key === '-') {
            state.lineNumber = '';
        }
        state.previousKeys = key;
    } else if (state.previousKeys === '' && (key === 'c' || key === 'r' || key === 'd')) {
        state.previousKeys = key;
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'CTRL_V') {
        state.mode = 'CTRL_V';
        state.visual = {
            row: state.row,
            col: state.col
        };
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'V') {
        state.mode = 'V';
        state.visual = {
            row: state.row,
            col: state.col
        };
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === 'n') {
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
    } else if (state.previousKeys === '' && key === 'N') {
        if (state.searchQuery !== '') {
            state.searching = true;
            searchBackForString(state, state.searchQuery);
            centerScreen(state);
            state.windowLineHorizontal = 0;
        }
    } else if (state.previousKeys === '' && key === '/') {
        state.mode = '/';
        state.searchQuery = '';
        state.search.row = state.row;
        state.search.col = state.col;
    } else if (state.previousKeys === '' && key === 'v') {
        state.mode = 'v';
        state.visual = {
            row: state.row,
            col: state.col
        };
        logCommand(true, state, key);
    } else if (state.previousKeys === '' && key === '}') {
        for (let i = state.row + 1; i < state.data.length; i += 1) {
            if (isEmptyRow(state, i)) {
                state.row = i;
                break;
            }
        }
    } else if (state.previousKeys === '' && key === '{') {
        for (let i = state.row - 1; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                state.row = i;
                break;
            }
        }
    } else if (state.previousKeys === '' && key === 'z') {
        centerScreen(state);
        state.windowLineHorizontal = 0;
        renderScreen(state, screen, undefined, true);
    } else if (state.previousKeys === '' && key === 'K') {
        if (state.data[state.row + 1] !== undefined) {
            state.col = state.data[state.row].length;
            state.data[state.row] += ' ' + state.data[state.row + 1].trim();
            state.data.splice(state.row + 1, 1);
        }
        logCommand(true, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === 'J') {
        if (state.data[state.row + 1] !== undefined) {
            state.col = state.data[state.row].length;
            state.data[state.row] += state.data[state.row + 1].trim();
            state.data.splice(state.row + 1, 1);
        }
        logCommand(true, state, key);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === 'CTRL_R') {
        if (state.data.length < 10000) {
            if (state.currentSnapshot + 1 < state.snapshots.length) {
                applySnapshot(state, state.currentSnapshot + 1, false);
                saveFile(state);
            }
        }
    } else if (state.previousKeys === '' && key === 'u') {
        if (state.data.length < 10000) {
            if (state.currentSnapshot - 1 >= 0) {
                applySnapshot(state, state.currentSnapshot - 1, true);
                saveFile(state);
            }
        }
    } else if (state.previousKeys === '' && key === '[') {
        previousSameIndentLevel(state, state.row);
    } else if (state.previousKeys === '' && key === ']') {
        nextSameIndentLevel(state, state.row);
    } else if (state.previousKeys === '' && key === '(') {
        previousLowerIndentLevel(state, state.row);
    } else if (state.previousKeys === '' && key === ')') {
        nextLowerIndentLevel(state, state.row);
    } else if (state.previousKeys === '' && key === '*') {
        const { beginning, end } = getCoorsInsideWord(state);
        setVisualHighlight(state, beginning, end);
        state.searchQuery = getInVisual(state);
        if (state.searchQuery !== '') {
            state.col += state.searchQuery.length + 1;
            searchForString(state, state.searchQuery);
            state.searching = true;
            centerScreen(state);
        }
    } else if (state.previousKeys === '' && key === 'q') {
        if (state.recording) {
            state.recording = false;
        } else {
            state.macro = [];
            state.recording = true;
        }
    } else if (state.previousKeys === '' && key === '=') {
        let indentLevel = state.row - 1 < 0 ? 0 : getIndentLevelFrom(state, state.row - 1);
        if (state.data[state.row].trim().startsWith(')') || state.data[state.row].trim().startsWith('}') || state.data[state.row].trim().startsWith('</')) {
            indentLevel = indentLevel - state.indentAmount >= 0 ? indentLevel - state.indentAmount : 0;
        }
        state.data[state.row] = ' '.repeat(indentLevel) + state.data[state.row].trim();
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === 'CTRL_G') {
        state.mode = 'g';
        let output = '';
        if (state.fileFinderQuery.length !== 0) {
            output = execSync(
                `git grep -n "${state.fileFinderQuery}" || true`,
                { maxBuffer: 1024 * 1024 * 1000 }
            ).toString();
        }
        state.fileFindingOutput = output.split('\n');
    } else if (state.previousKeys === '' && key === 'CTRL_N') {
        state.mode = 'f';
        state.gitFinding = false;
        state.fileFinderQuery = '';
        state.fileFinderIndex = 0;
        state.fileFindingOutput = [];
    } else if (state.previousKeys === '' && key === 'CTRL_Q') {
        state.mode = 'h';
        state.fileFindingOutput = [];
        for (let i = 0; i < state.snapshots.length; i += 1) {
            state.fileFindingOutput.push(i + ': (' + state.snapshots[i].date.diff(moment(), 'minutes') + ' min) ' + state.snapshots[i].commandHistory);
        }
        state.fileFinderIndex = state.currentSnapshot;
    } else if (state.previousKeys === '' && key === 'CTRL_P') {
        state.mode = 'f';
        state.gitFinding = true;
        state.fileFinderQuery = '';
        state.fileFinderIndex = 0;
        const output = execSync('fd -t f --hidden -E .git').toString();
        state.fileFindingOutput = output.split('\n');
    } else if (state.previousKeys === '' && key === '\'') {
        if (state.mark !== -1) {
            state.row = state.mark;
        }
    } else if (state.previousKeys === '' && key === '"') {
        if (state.mark2 !== -1) {
            state.row = state.mark2;
        }
    } else if (state.previousKeys === '' && key === '~') {
        let chr = state.data[state.row].substring(state.col, state.col + 1);
        chr = chr === chr.toUpperCase() ? chr.toLowerCase() : chr.toUpperCase();
        state.data[state.row] = state.data[state.row].substring(0, state.col)
            + chr
            + state.data[state.row].substring(state.col + 1);
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === 'M') {
        state.mark2 = state.row;
    } else if (state.previousKeys === '' && key === 'm') {
        state.mark = state.row;
    } else if (state.previousKeys === '' && key === '!') {
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
    } else if (state.previousKeys === '' && key === '%') {
        matchIt(state);
    } else if (state.previousKeys === '' && key === '?') {
        let systemPaste = getSystemPaste(state);
        if (systemPaste.startsWith('\n')) {
            systemPaste = systemPaste.substring(1);
        }
        state.searchQuery = systemPaste;
        state.searching = true;
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
    } else if (state.previousKeys === '' && key === ',') {
        state.allowCommandLogging = false;
        let chr = state.lastSearchCommand[0];
        if (chr !== undefined) {
            chr = chr === chr.toUpperCase() ? chr.toLowerCase() : chr.toUpperCase();
            sendKeys([chr, state.lastSearchCommand[1]], state, screen);
        }
        state.allowCommandLogging = true;
    } else if (state.previousKeys === '' && key === ';') {
        state.allowCommandLogging = false;
        sendKeys(state.lastSearchCommand, state, screen);
        state.allowCommandLogging = true;
    } else if (state.previousKeys === '' && key === ':') {
        state.mode = ':';
    } else if (state.previousKeys === '' && key === 'X') {
        state.harpoonIndexes = [];
        state.harpoonIndex = 0;
    } else if (state.previousKeys === '' && key === ' ') {
        if (!state.harpoonIndexes.includes(state.fileIndex)) {
            state.harpoonIndexes.push(state.fileIndex);
        } else {
            state.harpoonIndexes = state.harpoonIndexes.filter(((e) => (e !== state.fileIndex)));
        }
    } else if (state.previousKeys === '' && key === '@') {
        state.allowCommandLogging = false;
        sendKeys(state.macro, state, screen);
        state.allowCommandLogging = true;
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === '.') {
        state.allowCommandLogging = false;
        sendKeys(state.previousCommand, state, screen);
        state.allowCommandLogging = true;
        createSnapshot(state);
    } else if (state.previousKeys === '' && key === '\\') {
        refreshFile(state);
    }
    renderScreen(state, screen);
}

export {
    handleVimKeys
};
