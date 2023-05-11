/* eslint-disable import/no-cycle */
import { copyToClipboard, isAlphaNumeric } from './helper.js';

function isEmptyRow(state, row) {
    for (let i = 0; i < state.data[row].length; i += 1) {
        if (state.data[row].substring(i, i + 1) !== ' ') {
            return false;
        }
    }
    return true;
}

function firstNonSpace(state, row) {
    for (let i = 0; i < state.data[row]?.length; i += 1) {
        if (state.data[row].charAt(i) !== ' ') {
            return i;
        }
    }
}

function getIndentLevel(state, row) {
    return state.data[row].search(/\S|$/);
}

function previousSameIndentLevel(state, row) {
    const currentIndent = getIndentLevel(state, row);
    for (let i = row - 1; i >= 0; i -= 1) {
        if (getIndentLevel(state, i) === currentIndent) {
            state.row = i;
            state.col = firstNonSpace(state, i);
            break;
        }
    }
}

function nextSameIndentLevel(state, row) {
    const currentIndent = getIndentLevel(state, row);
    for (let i = row + 1; i < state.data.length; i += 1) {
        if (getIndentLevel(state, i) === currentIndent) {
            state.row = i;
            state.col = firstNonSpace(state, i);
            break;
        }
    }
}

function previousLowerIndentLevel(state, row) {
    const currentIndent = getIndentLevel(state, row);
    for (let i = row - 1; i >= 0; i -= 1) {
        if (getIndentLevel(state, i) === currentIndent - 4) {
            state.row = i;
            state.col = firstNonSpace(state, i);
            break;
        }
    }
}

function nextLowerIndentLevel(state, row) {
    const currentIndent = getIndentLevel(state, row);
    for (let i = row + 1; i < state.data.length; i += 1) {
        if (getIndentLevel(state, i) === currentIndent - 4) {
            state.row = i;
            state.col = firstNonSpace(state, i);
            break;
        }
    }
}

function uncomment(state, row) {
    const col = firstNonSpace(state, row);
    if (state.file.endsWith('.js')
        || state.file.endsWith('.jsx')
        || state.file.endsWith('.tsx')
        || state.file.endsWith('.ts')
        || state.file.endsWith('.java')
        || state.file.endsWith('.c')
        || state.file.endsWith('.cpp')
        || state.file.endsWith('.rs')) {
        state.data[row] = state.data[row].substring(0, col) + state.data[row].substring(col + 2);
    } else if (state.file.endsWith('.sh')
        || state.file.endsWith('.py')
        || state.file.endsWith('.bashrc')
        || state.file.endsWith('.zshrc')
        || state.file.endsWith('.env')) {
        state.data[row] = state.data[row].substring(0, col) + state.data[row].substring(col + 1);
        if (!isEmptyRow(state, row) && state.data[row].substring(col, col + 1) === ' ') {
            state.data[row] = state.data[row].substring(0, col) + state.data[row].substring(col + 1);
        }
    }
}

function comment(state, row, c) {
    const col = c === undefined ? firstNonSpace(state, row) : c;
    if (!isEmptyRow(state, row)) {
        if (state.file.endsWith('.js')
            || state.file.endsWith('.jsx')
            || state.file.endsWith('.tsx')
            || state.file.endsWith('.ts')
            || state.file.endsWith('.java')
            || state.file.endsWith('.c')
            || state.file.endsWith('.cpp')
            || state.file.endsWith('.rs')) {
            state.data[row] = state.data[row].substring(0, col) + '// ' + state.data[row].substring(col);
        } else if (state.file.endsWith('.sh')
            || state.file.endsWith('.py')
            || state.file.endsWith('.bashrc')
            || state.file.endsWith('.zshrc')
            || state.file.endsWith('.env')) {
            state.data[row] = state.data[row].substring(0, col) + '# ' + state.data[row].substring(col);
        }
    }
}

function isCommented(state, row) {
    const trimmed = state.data[row].trim();
    return trimmed.startsWith('//') || isEmptyRow(state, row);
}

function toBackward(state, key) {
    for (let i = state.col - 1; i >= 0; i -= 1) {
        if (state.data[state.row].substring(i, i + 1) === key) {
            return i + 1;
        }
    }
    return state.col;
}

function toForward(state, key) {
    for (let i = state.col + 1; i < state.data[state.row].length; i += 1) {
        if (state.data[state.row].substring(i, i + 1) === key) {
            return i - 1;
        }
    }
    return state.col;
}

function findBackward(state, key) {
    for (let i = state.col - 1; i >= 0; i -= 1) {
        if (state.data[state.row].substring(i, i + 1) === key) {
            return i;
        }
    }
    return state.col;
}

function findForward(state, key) {
    for (let i = state.col + 1; i < state.data[state.row].length; i += 1) {
        if (state.data[state.row].substring(i, i + 1) === key) {
            return i;
        }
    }
    return state.col;
}

function upHalfScreen(state) {
    for (let i = 0; i < process.stdout.rows / 2; i += 1) {
        if (state.row > 0) {
            state.row -= 1;
            if (state.windowLine > 0) {
                state.windowLine -= 1;
            }
        }
    }
}

function downHalfScreen(state) {
    for (let i = 0; i < process.stdout.rows / 2; i += 1) {
        if (state.row < state.data.length - 1) {
            state.row += 1;
            state.windowLine += 1;
        }
    }
}

function bottomOfFile(state) {
    while (state.row !== state.data.length - 1) {
        if (state.row < state.data.length - 1) {
            state.row += 1;
            if (state.row >= state.windowLine + process.stdout.rows) {
                state.windowLine += 1;
            }
        }
    }
}

function topOfFile(state) {
    while (state.row !== 0) {
        if (state.row > 0) {
            state.row -= 1;
            if (state.row < state.windowLine) {
                state.windowLine -= 1;
            }
        }
    }
}

function getCoorForwardWord(state) {
    let endOfWord = state.col;
    for (let i = state.col; i < state.data[state.row].length; i += 1) {
        if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
            endOfWord = i;
            break;
        } else if (i === state.data[state.row].length - 1) {
            endOfWord = state.data[state.row].length;
        }
    }
    return endOfWord;
}

function getCoorBeginningNextWord(state) {
    let hasHitNonAlphaNum = false;
    for (let i = state.col; i < state.data[state.row].length - 1; i += 1) {
        if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
            return i;
        } else if (!hasHitNonAlphaNum) {
            hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
        }
    }
    return state.col;
}

function getCoorBeginningLastWord(state) {
    let hasHitNonAlphaNum = isAlphaNumeric(state.data[state.row].substring(state.col - 1, state.col))
        && isAlphaNumeric(state.data[state.row].substring(state.col, state.col + 1));
    let { col } = state;
    for (let i = col; i >= 0; i -= 1) {
        if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
            col = i;
            break;
        } else if (!hasHitNonAlphaNum) {
            hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
        }
    }
    for (let i = col; i >= 0; i -= 1) {
        if (!isAlphaNumeric(state.data[state.row].charAt(i))) {
            break;
        }
        col = i;
    }
    return col;
}

function goToCoor(state, row) {
    if (row >= 0 && row < state.data.length) {
        state.row = row;
    }
}

function increaseIndentLevel(state, row) {
    state.data[row] = '    ' + state.data[row];
}

function decreaseIndentLevel(state, row) {
    let tempLine = state.data[row];
    let dont = false;
    for (let i = 3; i >= 0; i -= 1) {
        if (dont) {
            break;
        }
        for (let j = i; j >= 0; j -= 1) {
            if (tempLine.substring(j, j + 1) !== ' ') {
                dont = true;
            }
        }
        if (dont) {
            break;
        }
        if (tempLine.substring(i, i + 1) === ' ') {
            tempLine = tempLine.substring(0, i) + tempLine.substring(i + 1);
        } else {
            break;
        }
    }
    state.data[row] = tempLine;
}

function up(state) {
    if (state.row > 0) {
        state.row -= 1;
        if (state.row < state.windowLine) {
            state.windowLine -= 1;
        }
    }
}

function down(state) {
    if (state.row < state.data.length - 1) {
        state.row += 1;
        if (state.row >= state.windowLine + process.stdout.rows) {
            state.windowLine += 1;
        }
    }
}

function left(state) {
    if (state.col > state.data[state.row].length) {
        state.col = state.data[state.row].length;
    }
    if (state.col > 0) {
        state.col -= 1;
    }
}

function right(state) {
    if (state.col > state.data[state.row].length) {
        state.col = state.data[state.row].length;
    }
    if (state.col < state.data[state.row].length) {
        state.col += 1;
    }
}

function getCoorsInsideCharSame(state, key) {
    let beginning = state.col;
    for (let i = state.col; i >= 0; i -= 1) {
        if (state.data[state.row].substring(i, i + 1) === key) {
            beginning = i;
            break;
        } else if (i === 0) {
            beginning = -1;
        }
    }
    if (beginning === -1) {
        for (let i = state.col; i < state.data[state.row].length; i += 1) {
            if (state.data[state.row].substring(i, i + 1) === key) {
                beginning = i;
                break;
            } else if (i === state.data[state.row].length - 1) {
                beginning = -1;
            }
        }
    }
    let end = beginning;
    for (let i = beginning + 1; i < state.data[state.row].length; i += 1) {
        if (state.data[state.row].substring(i, i + 1) === key) {
            end = i;
            break;
        } else if (i === state.data[state.row].length - 1) {
            end = -1;
        }
    }
    return { beginning, end };
}

function getCoorsInsideCharDiff(state, openKey, closeKey) {
    let stack = 0;
    let beginning = state.col;
    for (let i = state.col; i >= 0; i -= 1) {
        if (state.data[state.row].substring(i, i + 1) === closeKey) {
            if (i !== state.col) {
                stack += 1;
            }
        } else if (state.data[state.row].substring(i, i + 1) === openKey) {
            if (stack === 0) {
                beginning = i;
                break;
            } else {
                stack -= 1;
            }
        } else if (i === 0) {
            beginning = -1;
        }
    }
    if (beginning === -1) {
        for (let i = state.col; i < state.data[state.row].length; i += 1) {
            if (state.data[state.row].substring(i, i + 1) === openKey) {
                beginning = i;
                break;
            } else if (i === state.data[state.row].length - 1) {
                beginning = -1;
            }
        }
    }
    let end = beginning;
    for (let i = beginning + 1; i < state.data[state.row].length; i += 1) {
        if (state.data[state.row].substring(i, i + 1) === openKey) {
            stack += 1;
        } else if (state.data[state.row].substring(i, i + 1) === closeKey) {
            if (stack === 0) {
                end = i;
                break;
            } else {
                stack -= 1;
            }
        } else if (i === state.data[state.row].length - 1) {
            end = -1;
        }
    }
    return { beginning, end };
}

function getCoorsInsideWord(state) {
    let beginning = state.col;
    for (let i = state.col; i >= 0; i -= 1) {
        if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
            beginning = i;
            break;
        } else if (i === 0) {
            beginning = -1;
        }
    }
    let end = beginning;
    for (let i = state.col; i < state.data[state.row].length; i += 1) {
        if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
            end = i;
            break;
        } else if (i === state.data[state.row].length - 1) {
            end = state.data[state.row].length;
        }
    }
    return { beginning, end };
}

function removeInsideAreaSameLine(state, beginning, end, mode) {
    if (beginning !== end) {
        state.data[state.row] = state.data[state.row].substring(0, beginning + 1) + state.data[state.row].substring(end);
        state.col = beginning + 1;
        state.mode = mode;
    }
}

function endOfLine(state, row) {
    return state.data[row].length - 1 >= 0 ? state.data[row].length - 1 : 0;
}

function copyInsideAreaSameLine(state, beginning, end) {
    if (beginning !== end && beginning !== -1 && end !== -1) {
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)], false);
        state.col = beginning + 1;
    }
}

function getIndentLevelFrom(state, row, inverse) {
    let indentLevel = getIndentLevel(state, row);
    if (inverse) {
        if (state.data[row].endsWith('}') || state.data[row].endsWith(')')) {
            indentLevel += 4;
        }
    } else {
        if (state.data[row].endsWith('{') || state.data[row].endsWith('(')) {
            indentLevel += 4;
        }
    }
    return indentLevel >= 0 ? indentLevel : 0;
}

function setVisualHighlight(state, beginning, end) {
    if (beginning !== end) {
        state.visual.col = beginning + 1;
        state.col = end - 1;
        state.visual.row = state.row;
    }
}

function deleteInVisual(state) {
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
}

function copyInVisual(state) {
    if (state.row === state.visual.row) {
        if (state.visual.col <= state.col) {
            copyToClipboard(state, [state.data[state.row].substring(state.visual.col, state.col + 1)], false);
        } else if (state.visual.col > state.col) {
            copyToClipboard(state, [state.data[state.row].substring(state.col, state.visual.col + 1)], false);
        }
    } else if (state.row > state.visual.row) {
        const newClipboard = [];
        newClipboard.push(state.data[state.visual.row].substring(state.visual.col));
        for (let i = state.visual.row + 1; i < state.row; i += 1) {
            newClipboard.push(state.data[i]);
        }
        newClipboard.push(state.data[state.row].substring(0, state.col + 1));
        copyToClipboard(state, newClipboard, false);
    } else if (state.row < state.visual.row) {
        const newClipboard = [];
        newClipboard.push(state.data[state.row].substring(state.col));
        for (let i = state.row + 1; i < state.visual.row; i += 1) {
            newClipboard.push(state.data[i]);
        }
        newClipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
        copyToClipboard(state, newClipboard, false);
    }
}

export {
    up,
    down,
    left,
    right,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    getIndentLevel,
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
    setVisualHighlight,
    copyInVisual,
    deleteInVisual,
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
};
