/* eslint-disable import/no-cycle */
import fs from 'fs';
import {
    adjustRow,
    applySnapshot,
    calcFileExplorerOutput,
    calcFileFinderOutput,
    calcGrepOutput,
    centerScreen,
    cleanup,
    copyAndRemoveRows,
    copyRows,
    copyToClipboard,
    createSnapshot,
    getFormattedLines,
    insertIndentedRow,
    isNumeric,
    isWritable,
    logCommand,
    pasteFromClipboardAfter,
    pasteFromClipboardBefore,
    processFile,
    refreshFile,
    renderScreen,
    saveFile,
    searchBackForString,
    searchForString,
    searchForStringNoWrap,
    setFileSearchOutput,
    trimTrailingWhitespace,
} from '../util/helper.js';
import { sendKeys } from '../util/sendKeys.js';
import {
    COMMAND,
    FILEEXPLORER,
    FILEFINDER,
    GREP,
    HISTORY,
    SEARCH,
    SHORTCUTS,
    TYPING,
    VISUAL,
    VISUALBLOCK,
    VISUALLINE,
} from '../util/modes.js';
import {
    findLastNonEmptyRow,
    findNextEmptyRow,
    bottomOfFile,
    copyInVisual,
    copyInsideAreaSameLine,
    decreaseIndentLevel,
    deleteInVisual,
    down,
    downHalfScreen,
    endOfLine,
    findBackward,
    findForward,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    getCoorForwardWord,
    getCoorsInsideCharDiff,
    getCoorsInsideCharSame,
    getCoorsInsideWord,
    getInVisual,
    goToCoor,
    increaseIndentLevel,
    isEmptyRow,
    left,
    matchIt,
    nextLowerIndentLevel,
    nextSameIndentLevel,
    previousLowerIndentLevel,
    previousSameIndentLevel,
    removeInsideAreaSameLine,
    right,
    setVisualHighlight,
    swapLeft,
    swapRight,
    toBackward,
    toForward,
    toggleComment,
    topOfFile,
    uncommentBlock,
    up,
    upHalfScreen,
} from '../util/movement.js';

function handleShortcutKeys(key, state, screen) {
    state.searching = false;
    if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === '-') {
        if (isNumeric(key)) {
            state.lineNumber += key;
            goToCoor(state, parseInt(state.lineNumber) - 1);
        } else if (key === 'ENTER') {
            cleanup(state, key, false, false, false, true);
        } else {
            cleanup(state, key, false, false, false, true);
            sendKeys([key], state, screen);
        }
    } else if (state.previousKeys + key === 'dw') {
        const endOfWord = getCoorForwardWord(state);
        copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)]);
        state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'dj') {
        const newClipboard = [''];
        newClipboard.push(state.data[state.row]);
        if (state.data[state.row + 1] !== undefined) {
            newClipboard.push(state.data[state.row + 1]);
        }
        copyToClipboard(state, newClipboard);
        state.data.splice(state.row, 1);
        state.data.splice(state.row, 1);
        state.col = firstNonSpace(state, state.row);
        cleanup(state, key, true, false, true, true);
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
        up(state);
        state.col = firstNonSpace(state, state.row);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'd' && (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a')) {
        state.previousKeys += key;
        cleanup(state, key, true, false, false, false);
    } else if (state.previousKeys + key === 'dd') {
        copyToClipboard(state, ['', state.data[state.row]]);
        state.data.splice(state.row, 1);
        state.col = firstNonSpace(state, state.row);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'df' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'dF' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'dt' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'dT' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'daw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'daW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'dap') {
        state.row = findLastNonEmptyRow(state, state.row);
        state.visual.row = findNextEmptyRow(state, state.row + 1);
        const startRow = Math.min(state.row, state.visual.row);
        const endRow = Math.max(state.row, state.visual.row);
        copyAndRemoveRows(state, startRow, endRow, true);
        adjustRow(state);
        firstNonSpace(state, state.row);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'da' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'da' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '>', '<');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'da' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'da' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'da' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'ds') {
        const beginning = state.col - 1;
        const end = state.col + state.searchQuery.length;
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'diw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'diW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'dip') {
        state.row = findLastNonEmptyRow(state, state.row);
        state.visual.row = findNextEmptyRow(state, state.row + 1);
        const startRow = Math.min(state.row, state.visual.row);
        const endRow = Math.max(state.row, state.visual.row);
        copyAndRemoveRows(state, startRow, endRow, false);
        adjustRow(state);
        firstNonSpace(state, state.row);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'di' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'di' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '>', '<');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'di' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'di' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'di' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, SHORTCUTS);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === 'yf' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = findForward(state, key);
        copyInVisual(state);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yF' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findBackward(state, key);
        copyInVisual(state);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yt' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = toForward(state, key);
        copyInVisual(state);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yT' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toBackward(state, key);
        copyInVisual(state);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yaw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yaW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yap') {
        state.row = findLastNonEmptyRow(state, state.row);
        state.visual.row = findNextEmptyRow(state, state.row + 1);
        const startRow = Math.min(state.row, state.visual.row);
        const endRow = Math.max(state.row, state.visual.row);
        copyRows(state, startRow, endRow, true);
        adjustRow(state);
        firstNonSpace(state, state.row);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'ya' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'ya' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '>', '<');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'ya' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'ya' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'ya' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyInsideAreaSameLine(state, beginning - 1, end + 1);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'ys') {
        const beginning = state.col - 1;
        const end = state.col + state.searchQuery.length;
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yiw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yiW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yip') {
        state.row = findLastNonEmptyRow(state, state.row);
        state.visual.row = findNextEmptyRow(state, state.row + 1);
        const startRow = Math.min(state.row, state.visual.row);
        const endRow = Math.max(state.row, state.visual.row);
        copyRows(state, startRow, endRow, false);
        adjustRow(state);
        firstNonSpace(state, state.row);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yi' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yi' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '>', '<');
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yi' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yi' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'yi' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyInsideAreaSameLine(state, beginning, end);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yj') {
        const newClipboard = [''];
        newClipboard.push(state.data[state.row]);
        if (state.data[state.row + 1]) {
            newClipboard.push(state.data[state.row + 1]);
        }
        copyToClipboard(state, newClipboard);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yk') {
        const newClipboard = [''];
        if (state.data[state.row - 1]) {
            newClipboard.push(state.data[state.row - 1]);
        }
        newClipboard.push(state.data[state.row]);
        copyToClipboard(state, newClipboard);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'yy') {
        copyToClipboard(state, ['', state.data[state.row]]);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'y' && (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a')) {
        state.previousKeys += key;
    } else if (state.previousKeys === 'c' && (key === 'i' || key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'a')) {
        state.previousKeys += key;
        cleanup(state, key, true, false, false, false);
    } else if (state.previousKeys + key === 'cj') {
        const newClipboard = [''];
        newClipboard.push(state.data[state.row]);
        if (state.data[state.row + 1]) {
            newClipboard.push(state.data[state.row + 1]);
        }
        copyToClipboard(state, newClipboard);
        state.data.splice(state.row, 1);
        state.data.splice(state.row, 1);
        insertIndentedRow(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
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
        insertIndentedRow(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'cw') {
        const endOfWord = getCoorForwardWord(state);
        copyToClipboard(state, [state.data[state.row].substring(state.col, endOfWord)]);
        state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'cc') {
        state.data.splice(state.row, 1);
        insertIndentedRow(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'cf' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'cF' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = findBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ct' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toForward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'cT' && isWritable(key)) {
        state.visual.row = state.row;
        state.visual.col = state.col;
        state.col = toBackward(state, key);
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'caw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'caW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end + 1)]);
        removeInsideAreaSameLine(state, beginning, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'cap') {
        state.row = findLastNonEmptyRow(state, state.row);
        state.visual.row = findNextEmptyRow(state, state.row + 1);
        const startRow = Math.min(state.row, state.visual.row);
        const endRow = Math.max(state.row, state.visual.row);
        copyAndRemoveRows(state, startRow, endRow, true);
        adjustRow(state);
        insertIndentedRow(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ca' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ca' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '>', '<');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ca' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ca' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ca' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning, end + 1)]);
        removeInsideAreaSameLine(state, beginning - 1, end + 1, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'cs') {
        const beginning = state.col - 1;
        const end = state.col + state.searchQuery.length;
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'ciw') {
        const { beginning, end } = getCoorsInsideWord(state);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'ciW') {
        const { beginning, end } = getCoorsInsideCharSame(state, ' ');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys + key === 'cip') {
        state.row = findLastNonEmptyRow(state, state.row);
        state.visual.row = findNextEmptyRow(state, state.row + 1);
        const startRow = Math.min(state.row, state.visual.row);
        const endRow = Math.max(state.row, state.visual.row);
        copyAndRemoveRows(state, startRow, endRow, false);
        adjustRow(state);
        insertIndentedRow(state);
        state.mode = TYPING;
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ci' && (key === '[' || key === ']' || key === 'd')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '[', ']');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ci' && (key === '<' || key === '>' || key === 't')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '>', '<');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ci' && (key === '(' || key === ')' || key === 'b')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ci' && (key === '{' || key === '}' || key === 'B')) {
        const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'ci' && (key === '\'' || key === '"' || key === '`')) {
        const { beginning, end } = getCoorsInsideCharSame(state, key);
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        removeInsideAreaSameLine(state, beginning, end, TYPING);
        cleanup(state, key, true, false, false, true);
    } else if (state.previousKeys === 'T' && isWritable(key)) {
        state.col = toBackward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['T', key];
        }
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 't' && isWritable(key)) {
        state.col = toForward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['t', key];
        }
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'F' && isWritable(key)) {
        state.col = findBackward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['F', key];
        }
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'f' && isWritable(key)) {
        state.col = findForward(state, key);
        if (state.allowCommandLogging) {
            state.lastSearchCommand = ['f', key];
        }
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys === 'r' && isWritable(key)) {
        state.data[state.row] = state.data[state.row].substring(0, state.col) + key + state.data[state.row].substring(state.col + 1);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'gg') {
        topOfFile(state);
        cleanup(state, key, false, false, false, true);
    } else if (state.previousKeys + key === 'gt') {
        trimTrailingWhitespace(state);
        logCommand(true, state, 'g');
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys + key === 'ge') {
        uncommentBlock(state, state.row);
        logCommand(true, state, 'g');
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === '' && key === 'e') {
        toggleComment(state, state.row);
        cleanup(state, key, true, false, true, true);
    } else if (state.previousKeys === '' && key === 'CTRL_O') {
        if (state.fileStack[state.fileStackIndex - 1]) {
            processFile(state, state.fileStack[state.fileStackIndex - 1], -1);
            state.fileStackIndex -= 1;
        }
    } else if (state.previousKeys === '' && key === 'TAB') {
        if (state.fileStack[state.fileStackIndex + 1]) {
            processFile(state, state.fileStack[state.fileStackIndex + 1], -1);
            state.fileStackIndex += 1;
        }
    } else if (state.previousKeys === '' && key === 'CTRL_W') {
        if (state.harpoonIndex - 1 >= 0) {
            state.harpoonIndex -= 1;
        }
        if (state.files[state.harpoonIndexes[state.harpoonIndex]] !== undefined) {
            if (fs.existsSync(state.files[state.harpoonIndexes[state.harpoonIndex]])) {
                processFile(state, state.files[state.harpoonIndexes[state.harpoonIndex]], -1);
            } else {
                state.harpoonIndexes.splice(state.harpoonIndex, 1);
                state.status = 'file not found';
            }
        }
    } else if (state.previousKeys === '' && key === 'CTRL_E') {
        if (state.harpoonIndex + 1 < state.harpoonIndexes.length) {
            state.harpoonIndex += 1;
        }
        if (state.files[state.harpoonIndexes[state.harpoonIndex]] !== undefined) {
            if (fs.existsSync(state.files[state.harpoonIndexes[state.harpoonIndex]])) {
                processFile(state, state.files[state.harpoonIndexes[state.harpoonIndex]], -1);
            } else {
                state.harpoonIndexes.splice(state.harpoonIndex, 1);
                state.status = 'file not found';
            }
        }
    } else if (state.previousKeys === '' && key === 'CTRL_X') {
        state.harpoonIndexes = state.harpoonIndexes.filter(((e) => (e !== state.fileIndex)));
        if (state.harpoonIndex - 1 >= 0) {
            state.harpoonIndex -= 1;
        }
        if (state.files[state.harpoonIndexes[state.harpoonIndex]] !== undefined) {
            processFile(state, state.files[state.harpoonIndexes[state.harpoonIndex]], -1);
        }
    } else if (state.previousKeys === '' && key === 'CTRL_F') {
        state.replacing = true;
        state.mode = SEARCH;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'BACKSPACE') {
        state.harpoonIndex = swapLeft({ array: state.harpoonIndexes, index: state.harpoonIndex });
    } else if (state.previousKeys === '' && key === 'CTRL_L') {
        state.harpoonIndex = swapRight({ array: state.harpoonIndexes, index: state.harpoonIndex });
    } else if (state.previousKeys === '' && key === 'CTRL_U') {
        upHalfScreen(state);
    } else if (state.previousKeys === '' && key === 'CTRL_D') {
        downHalfScreen(state);
    } else if (state.previousKeys === '' && key === 'CTRL_S') {
        state.mode = FILEEXPLORER;
        calcFileExplorerOutput(state);
    } else if (state.previousKeys === '' && (key === 'UP' || key === 'k')) {
        up(state);
    } else if (state.previousKeys === '' && (key === 'DOWN' || key === 'j')) {
        down(state);
    } else if (state.previousKeys === '' && (key === 'LEFT' || key === 'h')) {
        left(state);
    } else if (state.previousKeys === '' && (key === 'RIGHT' || key === 'l')) {
        right(state);
    } else if (state.previousKeys === '' && key === 'i') {
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'a') {
        right(state);
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'x') {
        if (state.col < state.data[state.row].length) {
            copyToClipboard(state, [state.data[state.row].substring(state.col, state.col + 1)]);
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + state.data[state.row].substring(state.col + 1);
        }
        cleanup(state, key, true, true, true, false);
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
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'w') {
        state.col = getCoorBeginningNextWord(state);
    } else if (state.previousKeys === '' && key === 'b') {
        state.col = getCoorBeginningLastWord(state);
    } else if (state.previousKeys === '' && key === 'A') {
        state.col = state.data[state.row].length;
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'S') {
        state.data[state.row] = '';
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'Y') {
        copyToClipboard(state, [state.data[state.row].substring(state.col)]);
    } else if (state.previousKeys === '' && key === 'C') {
        state.data[state.row] = state.data[state.row].substring(0, state.col);
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'D') {
        copyToClipboard(state, [state.data[state.row].substring(state.col)]);
        state.data[state.row] = state.data[state.row].substring(0, state.col);
        cleanup(state, key, true, true, true, false);
    } else if (state.previousKeys === '' && key === 's') {
        if (state.col < state.data[state.row].length) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + state.data[state.row].substring(state.col + 1);
        }
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'O') {
        insertIndentedRow(state);
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'o') {
        insertIndentedRow(state, true);
        down(state);
        state.mode = TYPING;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === '>') {
        increaseIndentLevel(state, state.row);
        state.col = firstNonSpace(state, state.row);
        cleanup(state, key, true, true, true, false);
    } else if (state.previousKeys === '' && key === '<') {
        decreaseIndentLevel(state, state.row);
        state.col = firstNonSpace(state, state.row);
        cleanup(state, key, true, true, true, false);
    } else if (state.previousKeys === '' && key === 'G') {
        bottomOfFile(state);
    } else if (state.previousKeys === '' && key === 'p') {
        if (pasteFromClipboardAfter(state)) {
            cleanup(state, key, true, true, true, false);
        }
    } else if (state.previousKeys === '' && key === 'P') {
        if (pasteFromClipboardBefore(state)) {
            cleanup(state, key, true, true, true, false);
        }
    } else if (state.previousKeys === '' && (key === 'f' || key === 't' || key === 'F' || key === 'T' || key === 'g' || key === '-' || key === 'y')) {
        if (key === '-') {
            state.lineNumber = '';
        }
        state.previousKeys = key;
    } else if (state.previousKeys === '' && (key === 'c' || key === 'r' || key === 'd')) {
        state.previousKeys = key;
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'CTRL_V') {
        state.mode = VISUALBLOCK;
        state.visual = {
            row: state.row,
            col: state.col
        };
        cleanup(state, key, true, true, false, false);
    } else if (state.previousKeys === '' && key === 'V') {
        state.mode = VISUALLINE;
        state.visual = {
            row: state.row,
            col: state.col
        };
        cleanup(state, key, true, true, false, false);
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
    } else if (state.previousKeys === '' && key === '?') {
        state.mode = SEARCH;
        state.searchQuery = '';
        state.reverseSearch = true;
        state.search.row = state.row;
        state.search.col = state.col;
    } else if (state.previousKeys === '' && key === '/') {
        state.mode = SEARCH;
        state.searchQuery = '';
        state.reverseSearch = false;
        state.search.row = state.row;
        state.search.col = state.col;
    } else if (state.previousKeys === '' && key === 'v') {
        state.mode = VISUAL;
        state.visual = {
            row: state.row,
            col: state.col
        };
        cleanup(state, key, true, true, false, false);
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
        cleanup(state, key, true, true, true, false);
    } else if (state.previousKeys === '' && key === 'J') {
        if (state.data[state.row + 1] !== undefined) {
            state.col = state.data[state.row].length;
            state.data[state.row] += state.data[state.row + 1].trim();
            state.data.splice(state.row + 1, 1);
        }
        cleanup(state, key, true, true, true, false);
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
    } else if (state.previousKeys === '' && key === '#') {
        const temprow = state.row;
        const tempcol = state.col;
        const { beginning, end } = getCoorsInsideWord(state);
        setVisualHighlight(state, beginning, end);
        state.grepQuery = getInVisual(state);
        if (state.grepQuery !== '') {
            state.grepIndex = 0;
            state.grepCursorPosition = state.grepQuery.length;
            state.mode = GREP;
            calcGrepOutput(state);
        }
        state.row = temprow;
        state.col = tempcol;
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
        const lines = getFormattedLines(state, state.row, state.row);
        state.data[state.row] = lines[0];
        cleanup(state, key, false, false, true, false);
    } else if (state.previousKeys === '' && key === 'CTRL_G') {
        state.mode = GREP;
        calcGrepOutput(state);
    } else if (state.previousKeys === '' && key === 'CTRL_N') {
        state.mode = FILEFINDER;
        state.gitFinding = false;
        state.fileFinderQuery = '';
        state.fileFinderCursorPosition = 0;
        state.fileFinderIndex = 0;
        setFileSearchOutput(state);
        calcFileFinderOutput(state);
    } else if (state.previousKeys === '' && key === 'CTRL_Q') {
        state.mode = HISTORY;
        state.fileFinderOutput = [];
        for (let i = 0; i < state.snapshots.length; i += 1) {
            state.fileFinderOutput.push(i + ': ' + state.snapshots[i].commandHistory);
        }
        state.fileFinderIndex = state.currentSnapshot;
    } else if (state.previousKeys === '' && key === 'CTRL_Y') {
        state.mode = FILEFINDER;
        state.gitFinding = true;
        calcFileFinderOutput(state);
    } else if (state.previousKeys === '' && key === 'CTRL_P') {
        state.mode = FILEFINDER;
        setFileSearchOutput(state);
        state.gitFinding = true;
        state.fileFinderQuery = '';
        state.fileFinderCursorPosition = 0;
        state.fileFinderIndex = 0;
        calcFileFinderOutput(state);
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
        cleanup(state, key, false, false, true, false);
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
    } else if (state.previousKeys === '' && key === ';') {
        state.allowCommandLogging = false;
        sendKeys(state.lastSearchCommand, state, screen);
        state.allowCommandLogging = true;
    } else if (state.previousKeys === '' && key === ':') {
        state.mode = COMMAND;
    } else if (state.previousKeys === '' && key === 'X') {
        state.harpoonIndexes = [];
        state.harpoonIndex = 0;
    } else if (state.previousKeys === '' && key === ' ') {
        if (!state.harpoonIndexes.includes(state.fileIndex)) {
            state.harpoonIndexes.push(state.fileIndex);
        } else {
            state.harpoonIndexes = state.harpoonIndexes.filter(((e) => (e !== state.fileIndex)));
        }
    } else if (state.previousKeys === '' && key === ',') {
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
    handleShortcutKeys
};
