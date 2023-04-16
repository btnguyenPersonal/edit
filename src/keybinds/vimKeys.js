/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import * as helper from '../util/helper.js';

function handleVimKeys(key, state, screen, term) {
    if (state.previousKeys === 'd') {
        if (key === 'w') {
            let endOfWord = state.col;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    endOfWord = i;
                    break;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(state.col, endOfWord));
            ncp.copy(state.clipboard.join('\n'));
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.previousKeys = '';
            helper.logCommand(false, state, key);
            helper.createSnapshot(state);
            helper.renderScreen(state, screen);
        } else if (key === 'j') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                state.clipboard.push(state.data[state.row + 1]);
            }
            state.data.splice(state.row, 1);
            state.data.splice(state.row, 1);
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            state.col = 0;
            state.previousKeys = '';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'k') {
            state.clipboard = [];
            if (state.data[state.row - 1]) {
                state.clipboard.push(state.data[state.row - 1]);
            }
            state.clipboard.push(state.data[state.row]);
            state.data.splice(state.row, 1);
            if (state.data[state.row - 1]) {
                state.data.splice(state.row - 1, 1);
            }
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
            }
            state.col = 0;
            state.previousKeys = '';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'i') {
            state.previousKeys += 'i';
            helper.logCommand(false, state, key);
        } else if (key === 'd') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            state.data.splice(state.row, 1);
            state.col = 0;
            if (state.row > state.data.length - 1) {
                state.row = state.data.length - 1;
            }
            state.previousKeys = '';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        }
    } else if (state.previousKeys === 'di') {
        if (key === 'w') {
            let beginningOfWord = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    beginningOfWord = i;
                    break;
                } else if (i === 0) {
                    beginningOfWord = -1;
                }
            }
            let endOfWord = state.col;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
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
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'y') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'j') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                state.clipboard.push(state.data[state.row + 1]);
            }
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            helper.logCommand(false, state, key);
        } else if (key === 'k') {
            state.clipboard = [];
            if (state.data[state.row - 1]) {
                state.clipboard.push(state.data[state.row - 1]);
            }
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            helper.logCommand(false, state, key);
        } else if (key === 'y') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
            ncp.copy(state.clipboard.join('\n'));
            helper.logCommand(false, state, key);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'r') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (helper.isWritable(key)) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + key
                + state.data[state.row].substring(state.col + 1);
            helper.renderScreen(state, screen);
        }
        helper.createSnapshot(state);
        helper.logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys.endsWith('g')) {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'g') {
            while (state.row !== 0) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.row < state.windowLine) {
                        state.windowLine -= 1;
                    }
                }
            }
            helper.renderScreen(state, screen);
        }
        helper.logCommand(false, state, key);
        state.previousKeys = '';
    } else if (state.previousKeys === 'c') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
            state.previousKeys = '';
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'i') {
            state.previousKeys += 'i';
            helper.logCommand(false, state, key);
        } else if (key === 'w') {
            let endOfWord = state.col;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    endOfWord = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfWord = state.data[state.row].length;
                }
            }
            state.clipboard = [];
            state.clipboardNewLine = false;
            state.clipboard.push(state.data[state.row].substring(state.col, endOfWord));
            ncp.copy(state.clipboard.join('\n'));
            state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(endOfWord);
            state.mode = 'i';
            state.previousKeys = '';
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
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
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        }
    } else if (state.previousKeys === 'ci') {
        if (key === 'w') {
            let beginningOfWord = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    beginningOfWord = i;
                    break;
                } else if (i === 0) {
                    beginningOfWord = -1;
                }
            }
            let endOfWord = beginningOfWord;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
                    endOfWord = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfWord = state.data[state.row].length - 1;
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
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
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
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            let stack = 0;
            let endOfArea = state.col;
            for (let i = state.col; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].substring(i, i + 1) === '}') {
                    endOfArea = i;
                    break;
                } else if (i === state.data[state.row].length - 1) {
                    endOfArea = state.data[state.row].length - 1;
                }
            }
            let beginningOfArea = endOfArea;
            for (let i = endOfArea; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === '{') {
                    beginningOfArea = i;
                    break;
                } else if (i === 0) {
                    beginningOfArea = -1;
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
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
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
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.mode === 'V') {
        if (key === 'CTRL_U') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.windowLine > 0) {
                        state.windowLine -= 1;
                    }
                }
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            helper.renderScreen(state, screen);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
                helper.renderScreen(state, screen);
            }
            helper.logCommand(false, state, key);
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                }
                helper.renderScreen(state, screen);
            }
            helper.logCommand(false, state, key);
        } else if (key === 'y') {
            if (state.row >= state.visualLine.row) {
                state.clipboard = [];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = true;
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                state.clipboard = [];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = true;
            }
            state.mode = 'n';
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'c') {
            if (state.row >= state.visualLine.row) {
                state.clipboard = [];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = true;
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                state.clipboard = [];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = true;
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            state.mode = 'i';
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'd') {
            if (state.row >= state.visualLine.row) {
                state.clipboard = [];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = true;
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                state.clipboard = [];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = true;
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'g') {
            state.previousKeys = 'g';
            helper.logCommand(false, state, key);
        } else if (key === 'G') {
            while (state.row !== state.data.length - 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    if (state.row >= state.windowLine + process.stdout.rows) {
                        state.windowLine += 1;
                    }
                }
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '<') {
            if (state.row >= state.visualLine.row) {
                for (let { row } = state.visualLine; row <= state.row; row += 1) {
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
                            if (state.col > 1) {
                                state.col -= 1;
                            }
                        } else {
                            break;
                        }
                    }
                    state.data[row] = tempLine;
                }
            } else if (state.row < state.visualLine.row) {
                for (let { row } = state; row <= state.visualLine.row; row += 1) {
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
                            if (state.col > 1) {
                                state.col -= 1;
                            }
                        } else {
                            break;
                        }
                    }
                    state.data[row] = tempLine;
                }
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '>') {
            if (state.row >= state.visualLine.row) {
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    state.data[i] = '    ' + state.data[i];
                }
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    state.data[i] = '    ' + state.data[i];
                }
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'x') {
            if (state.row >= state.visualLine.row) {
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'p' || key === 'P') {
            const systemPaste = ncp.paste().split('\n');
            if (state.clipboard !== systemPaste) {
                state.clipboard = systemPaste;
                state.clipboardNewLine = true;
            }
            if (state.clipboard.length > 0) {
                if (state.row >= state.visualLine.row) {
                    state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                    state.row = state.visualLine.row;
                } else if (state.row < state.visualLine.row) {
                    state.data.splice(state.row, state.visualLine.row - state.row + 1);
                }
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
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'ESCAPE') {
            state.mode = 'n';
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        }
    } else if (state.mode === 'v') {
        if (key === 'CTRL_U') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.windowLine > 0) {
                        state.windowLine -= 1;
                    }
                }
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
                helper.renderScreen(state, screen);
            }
            helper.logCommand(false, state, key);
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                }
                helper.renderScreen(state, screen);
            }
            helper.logCommand(false, state, key);
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'w') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i < state.data[state.row].length - 1; i += 1) {
                if (hasHitNonAlphaNum && helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !helper.isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'b') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i >= 0; i -= 1) {
                if (hasHitNonAlphaNum && helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !helper.isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            for (let i = state.col; i >= 0; i -= 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    break;
                }
                state.col = i;
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '0') {
            state.col = 0;
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'y') {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.visual.col, state.col + 1));
                    ncp.copy(state.clipboard.join('\n'));
                    state.clipboardNewLine = false;
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.col, state.visual.col + 1));
                    ncp.copy(state.clipboard.join('\n'));
                    state.clipboardNewLine = false;
                }
            } else if (state.row > state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.visual.row].substring(state.visual.col));
                for (let i = state.visual.row + 1; i < state.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.row].substring(0, state.col + 1));
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = false;
            } else if (state.row < state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.row].substring(state.col));
                for (let i = state.row + 1; i < state.visual.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = false;
            }
            state.mode = 'n';
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'p' || key === 'P') {
            const systemPaste = ncp.paste().split('\n');
            if (state.clipboard !== systemPaste) {
                state.clipboard = systemPaste;
                state.clipboardNewLine = false;
            }
            if (state.clipboard.length > 0) {
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
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'x') {
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
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'd') {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.visual.col, state.col + 1));
                    ncp.copy(state.clipboard.join('\n'));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.col, state.visual.col + 1));
                    ncp.copy(state.clipboard.join('\n'));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(state.visual.col + 1);
                }
            } else if (state.row > state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.visual.row].substring(state.visual.col));
                for (let i = state.visual.row + 1; i < state.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.row].substring(0, state.col + 1));
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = false;
                state.data[state.visual.row] = state.data[state.visual.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                state.data.splice(state.visual.row + 1, state.row - state.visual.row);
                state.row = state.visual.row;
                state.col = state.visual.col;
            } else if (state.row < state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.row].substring(state.col));
                for (let i = state.row + 1; i < state.visual.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = false;
                state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.visual.row].substring(state.visual.col + 1);
                state.data.splice(state.row + 1, state.visual.row - state.row);
            }
            state.mode = 'n';
            helper.createSnapshot(state);
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'c') {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.visual.col, state.col + 1));
                    ncp.copy(state.clipboard.join('\n'));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.col, state.visual.col + 1));
                    ncp.copy(state.clipboard.join('\n'));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(state.visual.col + 1);
                }
            } else if (state.row > state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.visual.row].substring(state.visual.col));
                for (let i = state.visual.row + 1; i < state.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.row].substring(0, state.col + 1));
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = false;
            } else if (state.row < state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.row].substring(state.col));
                for (let i = state.row + 1; i < state.visual.row; i += 1) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
                ncp.copy(state.clipboard.join('\n'));
                state.clipboardNewLine = false;
            }
            state.mode = 'i';
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'ESCAPE') {
            state.mode = 'n';
            helper.renderScreen(state, screen);
        }
    } else if (state.mode === 'n') {
        if (key === 'CTRL_U') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.windowLine > 0) {
                        state.windowLine -= 1;
                    }
                }
            }
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            helper.renderScreen(state, screen);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            helper.renderScreen(state, screen);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            helper.renderScreen(state, screen);
        } else if (key === 'i') {
            state.mode = 'i';
            helper.logCommand(true, state, key);
        } else if (key === 'a') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'x') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row].substring(state.col, state.col + 1));
            ncp.copy(state.clipboard.join('\n'));
            state.clipboardNewLine = false;
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            helper.renderScreen(state, screen);
        } else if (key === '0') {
            state.col = 0;
            helper.renderScreen(state, screen);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            helper.renderScreen(state, screen);
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
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'w') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i < state.data[state.row].length - 1; i += 1) {
                if (hasHitNonAlphaNum && helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !helper.isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            helper.renderScreen(state, screen);
        } else if (key === 'b') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i >= 0; i -= 1) {
                if (hasHitNonAlphaNum && helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !helper.isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            for (let i = state.col; i >= 0; i -= 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    break;
                }
                state.col = i;
            }
            helper.renderScreen(state, screen);
        } else if (key === 'A') {
            state.col = state.data[state.row].length;
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'S') {
            state.data[state.row] = '';
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'C') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'D') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row].substring(0, state.col));
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            helper.createSnapshot(state);
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 's') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'O') {
            let indentLevel = state.data[state.row].search(/\S|$/);
            if (state.data[state.row].endsWith('}') || state.data[state.row].endsWith(')')) {
                indentLevel += 4;
            }
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'o') {
            let indentLevel = state.data[state.row].search(/\S|$/);
            if (state.data[state.row].endsWith('{') || state.data[state.row].endsWith('(')) {
                indentLevel += 4;
            }
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
            state.row += 1;
            state.col = indentLevel;
            state.mode = 'i';
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '>') {
            state.data[state.row] = '    ' + state.data[state.row];
            state.col += 4;
            helper.createSnapshot(state);
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
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
            helper.createSnapshot(state);
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'g') {
            state.previousKeys = 'g';
            helper.logCommand(true, state, key);
        } else if (key === 'G') {
            while (state.row !== state.data.length - 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    if (state.row >= state.windowLine + process.stdout.rows) {
                        state.windowLine += 1;
                    }
                }
            }
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
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
                helper.createSnapshot(state);
                helper.logCommand(true, state, key);
                helper.renderScreen(state, screen);
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
                helper.createSnapshot(state);
                helper.logCommand(true, state, key);
                helper.renderScreen(state, screen);
            }
        } else if (key === 'r') {
            state.previousKeys = 'r';
            helper.logCommand(true, state, key);
        } else if (key === 'c') {
            state.previousKeys = 'c';
            helper.logCommand(true, state, key);
        } else if (key === 'd') {
            state.previousKeys = 'd';
            helper.logCommand(true, state, key);
        } else if (key === 'y') {
            state.previousKeys = 'y';
            helper.logCommand(true, state, key);
        } else if (key === '/') {
            state.previousKeys = '/';
            helper.logCommand(true, state, key);
        } else if (key === 'V') {
            state.mode = 'V';
            state.visualLine = {
                row: state.row
            };
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'v') {
            state.mode = 'v';
            state.visual = {
                row: state.row,
                col: state.col
            };
            helper.logCommand(true, state, key);
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_R') {
            if (state.currentSnapshot + 1 < state.snapshots.length) {
                helper.applySnapshot(state, state.currentSnapshot + 1);
            }
            helper.renderScreen(state, screen);
        } else if (key === 'u') {
            if (state.currentSnapshot - 1 >= 0) {
                helper.applySnapshot(state, state.currentSnapshot - 1);
            }
            helper.renderScreen(state, screen);
        } else if (key === '.') {
            state.allowCommandLogging = false;
            helper.sendKeys(state.previousCommand, state, screen, term);
            state.allowCommandLogging = true;
            helper.renderScreen(state, screen);
        }
    }
}

export {
    handleVimKeys
};
