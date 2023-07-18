/* eslint-disable import/no-cycle */
import {
    copyToClipboard,
    renderScreen,
    createSnapshot,
    pasteFromClipboardBefore,
    getSystemPaste,
    logCommand
} from '../util/helper.js';
import { sendKeys } from '../util/sendKeys.js';
import {
    up,
    down,
    left,
    right,
    getCoorBeginningNextWord,
    getCoorBeginningLastWord,
    endOfLine,
    firstNonSpace,
    increaseIndentLevel,
    decreaseIndentLevel,
    topOfFile,
    bottomOfFile,
    upHalfScreen,
    downHalfScreen,
    getIndentLevelFrom,
    previousSameIndentLevel,
    nextSameIndentLevel,
    previousLowerIndentLevel,
    nextLowerIndentLevel,
    isEmptyRow,
    isCommented,
    toggleComment,
    matchIt,
} from '../util/movement.js';

function handleVisualLineKeys(key, state, screen) {
    if (state.previousKeys === 'i' && key === 'p') {
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
                state.visual.row = i;
            }
        }
        for (let i = state.row + 1; i < state.data.length; i += 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.row = i;
            }
        }
    } else if (state.previousKeys === 'i' && key !== 'p') {
        state.previousKeys = '';
    }
    if (state.previousKeys !== 'i') {
        if (key === 'CTRL_U') {
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
            state.mode = 'n';
        } else if (key === 'c') {
            if (state.previousKeys === 'g') {
                let areAllCommented = true;
                let lowestIndent = isEmptyRow(state, state.row) ? 999 : firstNonSpace(state, state.row);
                if (state.row <= state.visual.row) {
                    for (let i = state.row + 1; i <= state.visual.row; i += 1) {
                        const currentIndent = isEmptyRow(state, i) ? 999 : firstNonSpace(state, i);
                        if (currentIndent < lowestIndent) {
                            lowestIndent = currentIndent;
                        }
                    }
                } else {
                    for (let i = state.row - 1; i >= state.visual.row; i -= 1) {
                        const currentIndent = isEmptyRow(state, i) ? 999 : firstNonSpace(state, i);
                        if (currentIndent < lowestIndent) {
                            lowestIndent = currentIndent;
                        }
                    }
                }
                if (lowestIndent !== 999) {
                    if (state.row >= state.visual.row) {
                        for (let i = state.visual.row; i <= state.row; i += 1) {
                            if (state.data[i].length !== 0 && !isCommented(state, i)) {
                                areAllCommented = false;
                                break;
                            }
                        }
                        if (areAllCommented) {
                            for (let i = state.visual.row; i <= state.row; i += 1) {
                                toggleComment(state, i, lowestIndent, false);
                            }
                        } else {
                            for (let i = state.visual.row; i <= state.row; i += 1) {
                                toggleComment(state, i, lowestIndent, true);
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
                                toggleComment(state, i, lowestIndent, false);
                            }
                        } else {
                            for (let i = state.row; i <= state.visual.row; i += 1) {
                                toggleComment(state, i, lowestIndent, true);
                            }
                        }
                    }
                }
                state.mode = 'n';
                createSnapshot(state);
                state.previousKeys = '';
            } else {
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
                state.mode = 'i';
            }
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
            state.mode = 'n';
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
            state.mode = 'n';
            state.allowCommandLogging = false;
            const startingPoint = state.row;
            if (state.visual.row < startingPoint) {
                for (let i = state.visual.row; i <= startingPoint; i += 1) {
                    sendKeys(state.macro, state, screen);
                    up(state);
                }
            } else {
                for (let i = startingPoint; i <= state.visual.row; i += 1) {
                    sendKeys(state.macro, state, screen);
                    down(state);
                }
            }
            state.allowCommandLogging = true;
            renderScreen(state, screen);
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
            const { beginningRow, endRow } = state.row <= state.visual.row
                ? { beginningRow: state.row, endRow: state.visual.row }
                : { beginningRow: state.visual.row, endRow: state.row };
            if (endRow < state.data.length - 1) {
                const temp = state.data[endRow + 1];
                state.data.splice(endRow + 1, 1);
                state.data.splice(beginningRow, 0, temp);
            }
            state.row += 1;
            state.visual.row += 1;
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
            createSnapshot(state);
        } else if (key === 'K') {
            const { beginningRow, endRow } = state.row <= state.visual.row
                ? { beginningRow: state.row, endRow: state.visual.row }
                : { beginningRow: state.visual.row, endRow: state.row };
            if (beginningRow > 0) {
                const temp = state.data[beginningRow - 1];
                state.data.splice(beginningRow - 1, 1);
                state.data.splice(endRow, 0, temp);
            }
            state.row -= 1;
            state.visual.row -= 1;
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
            createSnapshot(state);
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
            createSnapshot(state);
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
            state.mode = 'n';
            createSnapshot(state);
        } else if (key === 'p' || key === 'P') {
            let systemPaste = getSystemPaste(state);
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
            state.mode = 'n';
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
            state.mode = 'n';
        } else if (key === 'v') {
            state.mode = 'v';
        } else if (key === 'CTRL_V') {
            state.mode = 'CTRL_V';
        } else if (key === 'o') {
            const tempRow = state.row;
            const tempCol = state.col;
            state.row = state.visual.row;
            state.col = state.visual.col;
            state.visual.row = tempRow;
            state.visual.col = tempCol;
        }
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleVisualLineKeys
};
