/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import {
    copyToClipboard,
    isAlphaNumeric,
    isWritable,
    sendKeys,
    renderScreen,
    createSnapshot,
    applySnapshot,
    logCommand
} from '../util/helper.js';
import {
    getCoorForwardWord,
    topOfFile,
    bottomOfFile,
    upHalfScreen,
    downHalfScreen
} from '../util/movement.js';

function handleVimKeys(key, state, screen) {
    if (state.previousKeys === 'd') {
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
            if (state.data[state.row + 1]) {
                newClipboard.push(state.data[state.row + 1]);
            }
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.row, 1);
            state.data.splice(state.row, 1);
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            state.col = 0;
            state.previousKeys = '';
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
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
            }
            state.col = 0;
            state.previousKeys = '';
            createSnapshot(state);
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === 'i') {
            state.previousKeys += 'i';
            logCommand(false, state, key);
        } else if (key === 'd') {
            copyToClipboard(state, [state.data[state.row]], true);
            state.data.splice(state.row, 1);
            state.col = 0;
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            state.previousKeys = '';
            createSnapshot(state);
            logCommand(false, state, key);
            renderScreen(state, screen);
        }
    } else if (state.previousKeys === 'di') {
        if (key === 'w') {
            let beginningOfWord = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    beginningOfWord = i;
                    break;
                } else if (i === 0) {
                    beginningOfWord = -1;
                }
            }
            let endOfWord = state.col;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    endOfWord = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfWord = state.data[state.row].length;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfWord + 1, endOfWord));
            ncp.copy(state.clipboard.join('\n'));
            state.data[state.row] = state.data[state.row].substring(0, beginningOfWord + 1) + state.data[state.row].substring(endOfWord);
            state.col = beginningOfWord + 1;
            createSnapshot(state);
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '(' || key === ')' || key === 'b') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === ')') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '(') {
                    if (stack === 0) {
                        beginningOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === '(') {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '(') {
                    stack += 1;
                } else if (state.data[state.row].substring(i, i + 1) === ')') {
                    if (stack === 0) {
                        endOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfArea + 1) + state.data[state.row].substring(endOfArea);
                state.col = beginningOfArea + 1;
                createSnapshot(state);
            }
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '{') {
                    if (stack === 0) {
                        beginningOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === '{') {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '{') {
                    stack += 1;
                } else if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (stack === 0) {
                        endOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfArea + 1) + state.data[state.row].substring(endOfArea);
                state.col = beginningOfArea + 1;
                createSnapshot(state);
            }
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '\'' || key === '"') {
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === key) {
                    beginningOfArea = i;
                    break;
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === key) {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === key) {
                    endOfArea = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfArea + 1) + state.data[state.row].substring(endOfArea);
                state.col = beginningOfArea + 1;
                createSnapshot(state);
            }
            logCommand(false, state, key);
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'yi') {
        if (key === 'w') {
            let beginningOfWord = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    beginningOfWord = i;
                    break;
                } else if (i === 0) {
                    beginningOfWord = -1;
                }
            }
            let endOfWord = state.col;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    endOfWord = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfWord = state.data[state.row].length;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfWord + 1, endOfWord));
            ncp.copy(state.clipboard.join('\n'));
            state.col = beginningOfWord + 1;
            renderScreen(state, screen);
        } else if (key === '(' || key === ')' || key === 'b') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === ')') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '(') {
                    if (stack === 0) {
                        beginningOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === '(') {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '(') {
                    stack += 1;
                } else if (state.data[state.row].substring(i, i + 1) === ')') {
                    if (stack === 0) {
                        endOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.col = beginningOfArea + 1;
            }
            renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '{') {
                    if (stack === 0) {
                        beginningOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === '{') {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '{') {
                    stack += 1;
                } else if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (stack === 0) {
                        endOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.col = beginningOfArea + 1;
            }
            renderScreen(state, screen);
        } else if (key === '\'' || key === '"') {
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === key) {
                    beginningOfArea = i;
                    break;
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === key) {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === key) {
                    endOfArea = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.col = beginningOfArea + 1;
            }
            renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'y') {
        if (key === 'j') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                state.clipboard.push(state.data[state.row + 1]);
            }
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            logCommand(false, state, key);
            state.previousKeys = '';
        } else if (key === 'k') {
            state.clipboard = [];
            if (state.data[state.row - 1]) {
                state.clipboard.push(state.data[state.row - 1]);
            }
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            logCommand(false, state, key);
            state.previousKeys = '';
        } else if (key === 'y') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            logCommand(false, state, key);
            state.previousKeys = '';
        } else if (key === 'i') {
            state.previousKeys += 'i';
        }
    } else if (state.previousKeys === 'r') {
        if (isWritable(key)) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + key
                + state.data[state.row].substring(state.col + 1);
            renderScreen(state, screen);
        }
        createSnapshot(state);
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys.endsWith('g')) {
        if (key === 'g') {
            topOfFile(state);
            renderScreen(state, screen);
        }
        logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'c') {
        if (key === 'i') {
            state.previousKeys += 'i';
            logCommand(false, state, key);
        } else if (key === 'w') {
            const endOfWord = getCoorForwardWord(state);
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(state.col, endOfWord));
            ncp.copy(state.clipboard.join('\n'));
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.mode = 'i';
            state.previousKeys = '';
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === 'c') {
            let indentLevel = 0;
            if (state.row - 1 > 0) {
                indentLevel = state.data[state.row - 1].search(/\S|$/);
                if (state.data[state.row - 1].endsWith('{') || state.data[state.row - 1].endsWith('(')) {
                    indentLevel += 4;
                }
            }
            state.col = indentLevel;
            state.data[state.row] = ' '.repeat(indentLevel);
            state.mode = 'i';
            state.previousKeys = '';
            logCommand(false, state, key);
            renderScreen(state, screen);
        }
    } else if (state.previousKeys === 'ci') {
        if (key === 'w') {
            let beginningOfWord = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    beginningOfWord = i;
                    break;
                } else if (i === 0) {
                    beginningOfWord = -1;
                }
            }
            let endOfWord = beginningOfWord;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    endOfWord = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfWord = state.data[state.row].length;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfWord + 1, endOfWord));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfWord !== endOfWord) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfWord + 1) + state.data[state.row].substring(endOfWord);
                state.col = beginningOfWord + 1;
            }
            state.mode = 'i';
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '(' || key === ')' || key === 'b') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === ')') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '(') {
                    if (stack === 0) {
                        beginningOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === '(') {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '(') {
                    stack += 1;
                } else if (state.data[state.row].substring(i, i + 1) === ')') {
                    if (stack === 0) {
                        endOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfArea + 1) + state.data[state.row].substring(endOfArea);
                state.col = beginningOfArea + 1;
                state.mode = 'i';
            }
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '{') {
                    if (stack === 0) {
                        beginningOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === '{') {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '{') {
                    stack += 1;
                } else if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (stack === 0) {
                        endOfArea = i;
                        break;
                    } else {
                        stack -= 1;
                    }
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfArea + 1) + state.data[state.row].substring(endOfArea);
                state.col = beginningOfArea + 1;
                state.mode = 'i';
            }
            logCommand(false, state, key);
            renderScreen(state, screen);
        } else if (key === '\'' || key === '"') {
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === key) {
                    beginningOfArea = i;
                    break;
                } else if (i === 0) {
                    beginningOfArea = -1;
                }
            }
            if (beginningOfArea === -1) {
                for (let i = state.col; i < state.data[state.row].length; i += 1) {
                    if (state.data[state.row].substring(i, i + 1) === key) {
                        beginningOfArea = i;
                        break;
                    } else if (i === state.data[state.row].length - 1) {
                        beginningOfArea = -1;
                    }
                }
            }
            let endOfArea = beginningOfArea;
            for (let i = beginningOfArea + 1; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === key) {
                    endOfArea = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = -1;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(beginningOfArea + 1, endOfArea));
            ncp.copy(state.clipboard.join('\n'));
            if (beginningOfArea !== endOfArea && beginningOfArea !== -1 && endOfArea !== -1) {
                state.data[state.row] = state.data[state.row].substring(0, beginningOfArea + 1) + state.data[state.row].substring(endOfArea);
                state.col = beginningOfArea + 1;
                state.mode = 'i';
            }
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
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
                renderScreen(state, screen);
            }
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                }
                renderScreen(state, screen);
            }
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            renderScreen(state, screen);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            renderScreen(state, screen);
        } else if (key === 'i') {
            state.mode = 'i';
            logCommand(true, state, key);
        } else if (key === 'a') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'x') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row].substring(state.col, state.col + 1));
            ncp.copy(state.clipboard.join('\n'));
            state.clipboardNewLine = false;
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            renderScreen(state, screen);
        } else if (key === '0') {
            state.col = 0;
            renderScreen(state, screen);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            renderScreen(state, screen);
        } else if (key === 'I') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'w') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i < state.data[state.row].length - 1; i += 1) {
                if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            renderScreen(state, screen);
        } else if (key === 'b') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i >= 0; i -= 1) {
                if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            for (let i = state.col; i >= 0; i -= 1) {
                if (!isAlphaNumeric(state.data[state.row].charAt(i))) {
                    break;
                }
                state.col = i;
            }
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
            state.clipboard = [];
            state.clipboard.push(state.data[state.row].substring(0, state.col));
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            createSnapshot(state);
            logCommand(true, state, key);
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
            let indentLevel = state.data[state.row].search(/\S|$/);
            if (state.data[state.row].endsWith('}') || state.data[state.row].endsWith(')')) {
                indentLevel += 4;
            }
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'o') {
            let indentLevel = state.data[state.row].search(/\S|$/);
            if (state.data[state.row].endsWith('{') || state.data[state.row].endsWith('(')) {
                indentLevel += 4;
            }
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
            state.row += 1;
            state.col = indentLevel;
            state.mode = 'i';
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === '>') {
            state.data[state.row] = '    ' + state.data[state.row];
            state.col += 4;
            createSnapshot(state);
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === '<') {
            let tempLine = state.data[state.row];
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
                    if (state.col > 1) {
                        state.col -= 1;
                    }
                } else {
                    break;
                }
            }
            state.data[state.row] = tempLine;
            createSnapshot(state);
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'g') {
            state.previousKeys = 'g';
            logCommand(true, state, key);
        } else if (key === 'G') {
            bottomOfFile(state);
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'p') {
            state.clipboard = ncp.paste().split('\n');
            if (state.clipboard.length > 0) {
                if (state.clipboardNewLine) {
                    for (let i = state.clipboard.length - 1; i >= 0; i -= 1) {
                        state.data.splice(state.row + 1, 0, state.clipboard[i]);
                    }
                } else {
                    const cutoff = state.data[state.row].substring(state.col + 1);
                    state.data[state.row] = state.data[state.row].substring(0, state.col + 1) + state.clipboard[0];
                    let counterRow = state.row;
                    for (let i = state.clipboard.length - 1; i >= 1; i -= 1) {
                        state.data.splice(state.row + 1, 0, state.clipboard[i]);
                        counterRow += 1;
                    }
                    state.data[counterRow] += cutoff;
                }
                createSnapshot(state);
                logCommand(true, state, key);
                renderScreen(state, screen);
            }
        } else if (key === 'P') {
            const systemPaste = ncp.paste().split('\n');
            if (state.clipboard !== systemPaste) {
                state.clipboard = systemPaste;
            }
            if (state.clipboard.length > 0) {
                if (state.clipboardNewLine) {
                    for (let i = state.clipboard.length - 1; i >= 0; i -= 1) {
                        state.data.splice(state.row, 0, state.clipboard[i]);
                    }
                } else {
                    const cutoff = state.data[state.row].substring(state.col);
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.clipboard[0];
                    let counterRow = state.row;
                    for (let i = state.clipboard.length - 1; i >= 1; i -= 1) {
                        state.data.splice(state.row + 1, 0, state.clipboard[i]);
                        counterRow += 1;
                    }
                    state.data[counterRow] += cutoff;
                }
                createSnapshot(state);
                logCommand(true, state, key);
                renderScreen(state, screen);
            }
        } else if (key === 'c' || key === 'r' || key === 'd' || key === 'y' || key === '/') {
            state.previousKeys = key;
            logCommand(true, state, key);
        } else if (key === 'V') {
            state.mode = 'V';
            state.visualLine = {
                row: state.row
            };
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'v') {
            state.mode = 'v';
            state.visual = {
                row: state.row,
                col: state.col
            };
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'J') {
            if (state.data[state.row + 1] !== undefined) {
                state.col = state.data[state.row].length;
                state.data[state.row] += state.data[state.row + 1].trim();
                state.data.splice(state.row + 1, 1);
            }
            logCommand(true, state, key);
            renderScreen(state, screen);
        } else if (key === 'CTRL_R') {
            if (state.currentSnapshot + 1 < state.snapshots.length) {
                applySnapshot(state, state.currentSnapshot + 1);
            }
            renderScreen(state, screen);
        } else if (key === 'u') {
            if (state.currentSnapshot - 1 >= 0) {
                applySnapshot(state, state.currentSnapshot - 1);
            }
            renderScreen(state, screen);
        } else if (key === '.') {
            state.allowCommandLogging = false;
            sendKeys(state.previousCommand, state, screen);
            state.allowCommandLogging = true;
            renderScreen(state, screen);
        }
    }
}

export {
    handleVimKeys
};
