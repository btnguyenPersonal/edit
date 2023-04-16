/* eslint-disable import/no-cycle */
import { isAlphaNumeric } from './helper.js';

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
        if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
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
            break;
        } else if (!hasHitNonAlphaNum) {
            hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
        }
    }
    return state.col;
}

function getCoorBeginningLastWord(state) {
    let hasHitNonAlphaNum = false;
    let col = state.col;
    for (let i = state.col; i >= 0; i -= 1) {
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
            
function firstNonSpace(state, row) {
    for (let i = 0; i < state.data[row].length; i += 1) {
        if (state.data[row].charAt(i) !== ' ') {
            return i;
        }
    }
}

function increaseIndentLevel(state, row) {
    state.data[row] = '    ' + state.data[row];
}

function lowerIndentLevel(state, row) {
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

function getIndentLevel(state, row) {
    return state.data[row].search(/\S|$/);
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
    lowerIndentLevel,
    getCoorForwardWord,
    topOfFile,
    bottomOfFile,
    upHalfScreen,
    downHalfScreen,
    getCoorsInsideCharSame,
    getCoorsInsideCharDiff,
    getCoorsInsideWord,
    removeInsideAreaSameLine,
};

