/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import * as helper from '../util/helper.js';

function handleVisualKeys(key, state, screen, term) {
    if (state.previousKeys === 'vi') {
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
            if (beginningOfWord !== endOfWord) {
                state.visual.col = beginningOfWord + 1;
                state.col = endOfWord - 1;
            }
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
            if (beginningOfArea !== endOfArea) {
                state.visual.col = beginningOfArea + 1;
                state.col = endOfArea - 1;
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        } else if (key === '{' || key === '}' || key === 'B') {
            let stack = 0;
            let beginningOfArea = state.col;
            for (let i = state.col; i >= 0; i -= 1) {
                if (state.data[state.row].substring(i, i + 1) === '}') {
                    if (i !== state.col) {
                        stack += 1;
                    }
                } else if (state.data[state.row].substring(i, i + 1) === '}') {
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
            if (beginningOfArea !== endOfArea) {
                state.visual.col = beginningOfArea + 1;
                state.col = endOfArea - 1;
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
            if (beginningOfArea !== endOfArea) {
                state.visual.col = beginningOfArea + 1;
                state.col = endOfArea - 1;
            }
            helper.logCommand(false, state, key);
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (key === 'CTRL_U') {
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
    } else if (key === 'i') {
        state.previousKeys = 'vi';
        helper.logCommand(false, state, key);
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
        helper.renderScreen(state, screen);
    }
}

export {
    handleVisualKeys
};
