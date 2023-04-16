/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import * as helper from '../util/helper.js';

function handleVisualLineKeys(key, state, screen) {
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
}

export {
    handleVisualLineKeys
};
