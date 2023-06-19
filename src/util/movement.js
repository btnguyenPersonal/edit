/* eslint-disable import/no-cycle */
import { copyToClipboard, isAlphaNumeric } from './helper.js';

function swapLeft(obj) {
    const i = obj.index;
    const arr = obj.array;
    if (i <= 0) return obj.index;
    const temp = arr[i];
    arr[i] = arr[i - 1];
    arr[i - 1] = temp;
    obj.index = i - 1;
    return obj.index;
}

function swapRight(obj) {
    const i = obj.index;
    const arr = obj.array;
    if (i >= arr.length - 1) return obj.index;
    const temp = arr[i];
    arr[i] = arr[i + 1];
    arr[i + 1] = temp;
    obj.index = i + 1;
    return obj.index;
}

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
    return 0;
}

function getIndentLevel(state, row) {
    return state.data[row] !== undefined ? state.data[row].search(/\S|$/) : 0;
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

function getInsideOfIndentLevel(state) {
    const currentIndent = getIndentLevel(state, state.row);
    let beginning = state.row;
    let end = state.row;
    for (let i = state.row - 1; i >= 0; i -= 1) {
        if (!isEmptyRow(state, i) && getIndentLevel(state, i) < currentIndent) {
            break;
        } else {
            beginning = i;
        }
    }
    for (let i = state.row + 1; i < state.data.length; i += 1) {
        if (!isEmptyRow(state, i) && getIndentLevel(state, i) < currentIndent) {
            break;
        } else {
            end = i;
        }
    }
    return { beginning, end };
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
        if (getIndentLevel(state, i) === currentIndent - state.indentAmount) {
            state.row = i;
            state.col = firstNonSpace(state, i);
            break;
        }
    }
}

function nextLowerIndentLevel(state, row) {
    const currentIndent = getIndentLevel(state, row);
    for (let i = row + 1; i < state.data.length; i += 1) {
        if (getIndentLevel(state, i) === currentIndent - state.indentAmount) {
            state.row = i;
            state.col = firstNonSpace(state, i);
            break;
        }
    }
}

function isCommented(state, row) {
    if (!isEmptyRow(state, row)) {
        const extension = state.file.split('.').pop().toLowerCase();
        const trimmedLine = state.data[row].trim();
        switch (extension) {
            case 'java':
            case 'c':
            case 'cpp':
            case 'h':
            case 'rs':
            case 'cs':
            case 'Makefile':
            case 'php':
            case 'js':
            case 'swift':
            case 'kt':
            case 'go':
            case 'scala':
            case 'ts':
            case 'jsx':
            case 'tsx':
            case 'rust':
            case 'vb':
            case 'ejs':
            case 'lua':
            case 'coffee':
            case 'groovy':
            case 'matlab':
            case 'diff':
            case 'scss':
            case 'cfg':
            case 'htm':
            case 'conf':
            case 'styl':
            case 'adoc':
            case 'cshtml':
            case 'aspx':
            case 'pug':
            case 'json':
            case 'json5':
            case 'dockerfile':
            case 'graphql':
            case 'iml':
            case 'twig':
            case 'bat':
            case 'phtml':
            case 'fish':
            case 'cake':
            case 'gd':
            case 'nim':
                return trimmedLine.startsWith('//');
            case 'sh':
            case 'bashrc':
            case 'zshrc':
            case 'yaml':
            case 'yml':
            case 'ini':
            case 'py':
            case 'rb':
            case 'gitignore':
            case 'pl':
            case 'r':
            case 'cobol':
            case 'properties':
            case 'cson':
            case 'env':
                return trimmedLine.startsWith('#');
            case 'css':
                return trimmedLine.startsWith('/*') && trimmedLine.endsWith('*/');
            case 'html':
            case 'xml':
            case 'markdown':
            case 'md':
                return trimmedLine.startsWith('<!--') && trimmedLine.endsWith('-->');
            default:
                return false;
        }
    }
}

function toggleComment(state, row, column, forceComment) {
    if (!isEmptyRow(state, row)) {
        const extension = state.file.split('.').pop().toLowerCase();
        const trimmedLine = state.data[row].trim();
        const leadingWhitespace = state.data[row].match(/^\s*/)[0];
        switch (extension) {
            case 'java':
            case 'c':
            case 'cpp':
            case 'h':
            case 'rs':
            case 'cs':
            case 'Makefile':
            case 'php':
            case 'js':
            case 'swift':
            case 'kt':
            case 'go':
            case 'scala':
            case 'ts':
            case 'jsx':
            case 'tsx':
            case 'rust':
            case 'vb':
            case 'ejs':
            case 'lua':
            case 'coffee':
            case 'groovy':
            case 'matlab':
            case 'diff':
            case 'scss':
            case 'cfg':
            case 'htm':
            case 'conf':
            case 'styl':
            case 'adoc':
            case 'cshtml':
            case 'aspx':
            case 'pug':
            case 'json':
            case 'json5':
            case 'dockerfile':
            case 'graphql':
            case 'iml':
            case 'twig':
            case 'bat':
            case 'phtml':
            case 'fish':
            case 'cake':
            case 'gd':
            case 'nim':
                if (forceComment !== true && trimmedLine.startsWith('// ')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(3);
                    break;
                } else if (forceComment !== true && trimmedLine.startsWith('//')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(2);
                    break;
                } else if (forceComment !== false && column !== undefined && column >= 0 && column <= state.data[row].length) {
                    state.data[row] = state.data[row].slice(0, column) + '// ' + state.data[row].slice(column);
                    break;
                } else if (forceComment !== false) {
                    state.data[row] = leadingWhitespace + '// ' + trimmedLine;
                    break;
                } else {
                    break;
                }
            case 'sh':
            case 'bashrc':
            case 'zshrc':
            case 'yaml':
            case 'yml':
            case 'ini':
            case 'py':
            case 'rb':
            case 'gitignore':
            case 'pl':
            case 'r':
            case 'cobol':
            case 'properties':
            case 'cson':
            case 'env':
                if (forceComment !== true && trimmedLine.startsWith('# ')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(2);
                    break;
                } else if (forceComment !== true && trimmedLine.startsWith('#')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(1);
                    break;
                } else if (forceComment !== false && column !== undefined && column >= 0 && column <= state.data[row].length) {
                    state.data[row] = state.data[row].slice(0, column) + '# ' + state.data[row].slice(column);
                    break;
                } else if (forceComment !== false) {
                    state.data[row] = leadingWhitespace + '# ' + trimmedLine;
                    break;
                } else {
                    break;
                }
            case 'css':
                if (forceComment !== true && trimmedLine.startsWith('/* ') && trimmedLine.endsWith(' */')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(3, -3);
                    break;
                } else if (forceComment !== true && trimmedLine.startsWith('/*') && trimmedLine.endsWith('*/')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(2, -2);
                    break;
                } else if (forceComment !== false && column !== undefined && column >= 0 && column <= state.data[row].length) {
                    state.data[row] = state.data[row].slice(0, column) + '/* ' + state.data[row].slice(column) + ' */';
                    break;
                } else if (forceComment !== false) {
                    state.data[row] = leadingWhitespace + '/* ' + trimmedLine + ' */';
                    break;
                } else {
                    break;
                }
            case 'html':
            case 'xml':
            case 'markdown':
            case 'md':
                if (forceComment !== true && trimmedLine.startsWith('<!-- ') && trimmedLine.endsWith(' -->')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(5, -4);
                    break;
                } else if (forceComment !== true && trimmedLine.startsWith('<!--') && trimmedLine.endsWith('-->')) {
                    state.data[row] = leadingWhitespace + trimmedLine.slice(4, -3);
                    break;
                } else if (forceComment !== false && column !== undefined && column >= 0 && column <= state.data[row].length) {
                    state.data[row] = state.data[row].slice(0, column) + '<!-- ' + state.data[row].slice(column) + ' -->';
                    break;
                } else if (forceComment !== false) {
                    state.data[row] = leadingWhitespace + '<!-- ' + trimmedLine + ' -->';
                    break;
                } else {
                    break;
                }
            default:
                break;
        }
    }
}

function uncommentBlock(state, row) {
    for (let i = row; i < state.data.length; i += 1) {
        if (isCommented(state, i)) {
            row = i;
            state.row = i;
            break;
        } else {
            row = i;
        }
    }
    for (let i = row; i >= 0; i -= 1) {
        if (isCommented(state, i)) {
            toggleComment(state, i);
        } else {
            break;
        }
    }
    for (let i = row + 1; i < state.data.length; i += 1) {
        if (isCommented(state, i)) {
            toggleComment(state, i);
        } else {
            break;
        }
    }
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
            if (state.row >= state.windowLine + process.stdout.rows - 1) {
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
    for (let i = state.col; i < state.data[state.row].length; i += 1) {
        if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
            return i;
        } else if (!hasHitNonAlphaNum) {
            hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
        }
    }
    return state.col;
}

function getCoorBeginningLastWord(state) {
    if (state.col > state.data[state.row].length - 1 && state.data[state.row].length > 0) {
        state.col = state.data[state.row].length - 1;
    }
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
        state.row = row - 1 >= 0 ? row : 0;
    }
}

function increaseIndentLevel(state, row) {
    state.data[row] = ' '.repeat(state.indentAmount) + state.data[row];
}

function decreaseIndentLevel(state, row) {
    let tempLine = state.data[row];
    let dont = false;
    for (let i = state.indentAmount - 1; i >= 0; i -= 1) {
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
        if (state.row >= state.windowLine + process.stdout.rows - 1) {
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
    if (beginning !== end) {
        copyToClipboard(state, [state.data[state.row].substring(beginning + 1, end)]);
        state.col = beginning + 1;
    }
}

function getIndentLevelFrom(state, row, inverse) {
    let indentLevel = getIndentLevel(state, row);
    if (inverse) {
        if (state.data[row].trim().startsWith('}')
            || state.data[row].trim().startsWith(')')
            || state.data[row].trim().startsWith('</')
        ) {
            indentLevel += state.indentAmount;
        }
    } else {
        if (state.data[row].trim().endsWith('{')
            || state.data[row].trim().endsWith('(')
            || (state.data[row].trim().startsWith('<') && !(state.data[row].trim().startsWith('</')))
        ) {
            if (!state.data[row].trim().endsWith('/>')) {
                indentLevel += state.indentAmount;
            }
        }
    }
    return indentLevel >= 0 ? indentLevel : 0;
}

function setVisualBlockHighlight(state, beginning, end) {
    if (beginning !== end) {
        state.visual.col = beginning + 1;
        state.col = end - 1;
        state.visual.row = state.row;
    }
}

function copyInVisualBlock(state) {
    const newClipboard = ['\n'];
    for (let i = Math.min(state.row, state.visual.row); i < Math.max(state.row, state.visual.row) + 1; i += 1) {
        if (state.visual.col <= state.col) {
            newClipboard.push(state.data[i].substring(state.visual.col, state.col + 1));
        } else if (state.visual.col > state.col) {
            newClipboard.push(state.data[i].substring(state.col, state.visual.col + 1));
        }
    }
    copyToClipboard(state, newClipboard, true);
}

function deleteInVisualBlock(state) {
    for (let i = Math.min(state.row, state.visual.row); i < Math.max(state.row, state.visual.row) + 1; i += 1) {
        if (state.visual.col <= state.col) {
            state.data[i] = state.data[i].substring(0, state.visual.col) + state.data[i].substring(state.col + 1);
        } else if (state.visual.col > state.col) {
            state.data[i] = state.data[i].substring(0, state.col) + state.data[i].substring(state.visual.col + 1);
        }
    }
    if (state.visual.row <= state.row) {
        state.row = state.visual.row;
    }
    if (state.visual.col <= state.col) {
        state.col = state.visual.col;
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

function getInVisual(state) {
    if (state.row === state.visual.row) {
        if (state.visual.col <= state.col) {
            return state.data[state.row].substring(state.visual.col, state.col + 1);
        } else if (state.visual.col > state.col) {
            return state.data[state.row].substring(state.col, state.visual.col + 1);
        }
    } else {
        return '';
    }
}

function copyInVisual(state) {
    if (state.row === state.visual.row) {
        if (state.visual.col <= state.col) {
            copyToClipboard(state, [state.data[state.row].substring(state.visual.col, state.col + 1)]);
        } else if (state.visual.col > state.col) {
            copyToClipboard(state, [state.data[state.row].substring(state.col, state.visual.col + 1)]);
        }
    } else if (state.row > state.visual.row) {
        const newClipboard = [];
        newClipboard.push(state.data[state.visual.row].substring(state.visual.col));
        for (let i = state.visual.row + 1; i < state.row; i += 1) {
            newClipboard.push(state.data[i]);
        }
        newClipboard.push(state.data[state.row].substring(0, state.col + 1));
        copyToClipboard(state, newClipboard);
    } else if (state.row < state.visual.row) {
        const newClipboard = [];
        newClipboard.push(state.data[state.row].substring(state.col));
        for (let i = state.row + 1; i < state.visual.row; i += 1) {
            newClipboard.push(state.data[i]);
        }
        newClipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
        copyToClipboard(state, newClipboard);
    }
}

function setAroundVisualHighlight(state, beginning, end) {
    if (beginning !== end && beginning !== -1 && end !== -1) {
        state.visual.col = beginning;
        state.col = end;
        state.visual.row = state.row;
    }
}

function setVisualHighlight(state, beginning, end) {
    if (beginning !== end && beginning !== -1 && end !== -1) {
        state.visual.col = beginning + 1;
        state.col = end - 1;
        state.visual.row = state.row;
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
    setVisualBlockHighlight,
    copyInVisualBlock,
    deleteInVisualBlock,
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
    previousSameIndentLevel,
    nextSameIndentLevel,
    previousLowerIndentLevel,
    nextLowerIndentLevel,
    goToCoor,
    getInVisual,
    toggleComment,
    getInsideOfIndentLevel,
    uncommentBlock,
    setAroundVisualHighlight,
    swapLeft,
    swapRight,
};
