/* eslint-disable import/no-cycle */
import {
    createSnapshot,
    isWritable,
    getFormattedLines,
    logCommand,
} from '../util/helper.js';
import { render } from '../util/render.js';
import {
    MULTICURSOR,
    SHORTCUTS,
    VISUAL,
    VISUALLINE,
} from '../util/modes.js';
import {
    bottomOfFile,
    commentVisualLines,
    copyInVisualBlock,
    deleteInVisualBlock,
    down,
    downHalfScreen,
    endOfLine,
    findBackward,
    findForward,
    getCoorEndNextWord,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    getCoorsInsideCharDiff,
    getCoorsInsideCharSame,
    getCoorsInsideWord,
    left,
    matchIt,
    right,
    setVisualBlockHighlight,
    toBackward,
    toForward,
    topOfFile,
    up,
    upHalfScreen,
} from '../util/movement.js';

function handleVisualBlockKeys(key, state, screen) {
    if (state.previousKeys === 'T') {
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
    } else if (state.previousKeys === 'i') {
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
        }
        state.previousKeys = '';
    } else if (key === 'CTRL_U') {
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
    } else if (state.previousKeys === '' && key === 'E') {
        state.col = getCoorEndNextWord(state);
    } else if (key === 'w' || key === 'W') {
        state.col = getCoorBeginningNextWord(state);
    } else if (key === 'b' || key === 'B') {
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
        state.mode = SHORTCUTS;
    } else if (key === 'x') {
        deleteInVisualBlock(state);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === '%') {
        matchIt(state);
    } else if (key === 'd') {
        copyInVisualBlock(state);
        deleteInVisualBlock(state);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    } else if (key === 'e') {
        commentVisualLines(state, Math.min(state.row, state.visual.row), Math.max(state.row, state.visual.row));
        state.mode = SHORTCUTS;
        createSnapshot(state);
        state.previousKeys = '';
    } else if (key === 'c') {
        const c = Math.min(state.col, state.visual.col);
        const rMin = Math.min(state.row, state.visual.row);
        const rMax = Math.max(state.row, state.visual.row);
        copyInVisualBlock(state);
        deleteInVisualBlock(state);
        state.col = c;
        state.row = rMin;
        state.visual.row = rMax;
        state.mode = MULTICURSOR;
    } else if (key === '=') {
        const start = Math.min(state.row, state.visual.row);
        const end = Math.max(state.row, state.visual.row);
        const lines = getFormattedLines(state, start, end);
        for (let i = 0; i <= end - start; i += 1) {
            state.data[start + i] = lines[i];
        }
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
    } else if (key === 'f' || key === 'F' || key === 't' || key === 'T' || key === 'i') {
        state.previousKeys += key;
    } else if (key === 'A') {
        state.col = Math.max(state.col, state.visual.col);
        state.mode = MULTICURSOR;
        right(state);
    } else if (key === 'I') {
        state.col = Math.min(state.col, state.visual.col);
        state.mode = MULTICURSOR;
    } else if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
    } else if (key === 'v') {
        state.mode = VISUAL;
    } else if (key === 'V') {
        state.mode = VISUALLINE;
    } else if (key === 'o') {
        const tempRow = state.row;
        const tempCol = state.col;
        state.row = state.visual.row;
        state.col = state.visual.col;
        state.visual.row = tempRow;
        state.visual.col = tempCol;
    }
    logCommand(false, state, key);
    render(state, screen);
}

export {
    handleVisualBlockKeys
};
