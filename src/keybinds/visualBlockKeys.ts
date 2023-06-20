/* eslint-disable import/no-cycle */
import {
    renderScreen,
    createSnapshot,
    isWritable,
    logCommand
} from '../util/helper.ts';
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
    setVisualBlockHighlight,
    copyInVisualBlock,
    deleteInVisualBlock,
    endOfLine,
    findForward,
    findBackward,
    increaseIndentLevel,
    decreaseIndentLevel,
    getIndentLevelFrom,
    toForward,
    toBackward,
    isEmptyRow,
    isCommented,
    toggleComment,
} from '../util/movement.ts';

function handleVisualBlockKeys(key, state, screen) {
    if (state.previousKeys === 'i') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            setVisualBlockHighlight(state, beginning, end);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            setVisualBlockHighlight(state, beginning, end);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            setVisualBlockHighlight(state, beginning, end);
        } else if (key === '\'' || key === '"') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            setVisualBlockHighlight(state, beginning, end);
        } else if (key === 'p') {
            for (let i = state.row - 1; i >= 0; i -= 1) {
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
    } else if (key === 'y') {
        copyInVisualBlock(state);
        if (state.row >= state.visual.row) {
            state.row = state.visual.row;
        }
        if (state.col >= state.visual.col) {
            state.col = state.visual.col;
        }
        state.mode = 'n';
    } else if (key === 'x') {
        deleteInVisualBlock(state);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'd') {
        copyInVisualBlock(state);
        deleteInVisualBlock(state);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'c') {
        if (state.previousKeys === 'g') {
            let areAllCommented = true;
            if (state.row >= state.visual.row) {
                for (let i = state.visual.row; i <= state.row; i += 1) {
                    if (!isCommented(state, i)) {
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
                    if (!isCommented(state, i)) {
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
            const c = Math.min(state.col, state.visual.col);
            const rMin = Math.min(state.row, state.visual.row);
            const rMax = Math.max(state.row, state.visual.row);
            copyInVisualBlock(state);
            deleteInVisualBlock(state);
            state.col = c;
            state.row = rMin;
            state.visual.row = rMax;
            state.mode = 'MULTI_CURSOR';
        }
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
        state.mode = 'n';
        createSnapshot(state);
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
    } else if (key === 'g') {
        if (state.previousKeys === 'g') {
            topOfFile(state);
            state.previousKeys = '';
        } else {
            state.previousKeys = 'g';
        }
    } else if (key === 'G') {
        bottomOfFile(state);
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
    } else if (key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'i') {
        state.previousKeys += key;
    } else if (key === 'A') {
        state.col = Math.max(state.col, state.visual.col);
        state.mode = 'MULTI_CURSOR';
        right(state);
    } else if (key === 'I') {
        state.col = Math.min(state.col, state.visual.col);
        state.mode = 'MULTI_CURSOR';
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
    } else if (key === 'v') {
        state.mode = 'v';
    } else if (key === 'V') {
        state.mode = 'V';
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
    handleVisualBlockKeys
};
