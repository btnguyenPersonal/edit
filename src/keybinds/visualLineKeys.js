/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import {
    copyToClipboard,
    renderScreen,
    createSnapshot,
    logCommand
} from '../util/helper.js';
import { sendKeys } from '../util/sendKeys.js';
import {
    up,
    down,
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
        state.visualLine.row = state.row;
        for (let i = state.row; i >= 0; i -= 1) {
            if (isEmptyRow(state, i)) {
                break;
            } else {
                state.visualLine.row = i;
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
        } else if (key === 'y') {
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
            }
            state.mode = 'n';
        } else if (key === 'c') {
            if (state.previousKeys === 'g') {
                let areAllCommented = true;
                let lowestIndent = firstNonSpace(state, state.row);
                for (let i = 0; i < state.visualLine.row; i += 1) {
                    const currentIndent = firstNonSpace(state, state.row);
                    if (currentIndent < lowestIndent) {
                        lowestIndent = currentIndent;
                    }
                }
                if (state.row >= state.visualLine.row) {
                    for (let i = state.visualLine.row; i <= state.row; i += 1) {
                        if (state.data[i].length !== 0 && !isCommented(state, i)) {
                            areAllCommented = false;
                            break;
                        }
                    }
                    if (areAllCommented) {
                        for (let i = state.visualLine.row; i <= state.row; i += 1) {
                            toggleComment(state, i, undefined, false);
                        }
                    } else {
                        for (let i = state.visualLine.row; i <= state.row; i += 1) {
                            toggleComment(state, i, lowestIndent, true);
                        }
                    }
                    state.row = state.visualLine.row;
                } else if (state.row < state.visualLine.row) {
                    for (let i = state.row; i <= state.visualLine.row; i += 1) {
                        if (state.data[i].length !== 0 && !isCommented(state, i)) {
                            areAllCommented = false;
                            break;
                        }
                    }
                    if (areAllCommented) {
                        for (let i = state.row; i <= state.visualLine.row; i += 1) {
                            toggleComment(state, i, undefined, false);
                        }
                    } else {
                        for (let i = state.row; i <= state.visualLine.row; i += 1) {
                            toggleComment(state, i, lowestIndent, true);
                        }
                    }
                }
                state.mode = 'n';
                createSnapshot(state);
                state.previousKeys = '';
            } else {
                if (state.row >= state.visualLine.row) {
                    const newClipboard = ['\n'];
                    for (let i = state.visualLine.row; i <= state.row; i += 1) {
                        newClipboard.push(state.data[i]);
                    }
                    copyToClipboard(state, newClipboard);
                    state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                    state.row = state.visualLine.row;
                } else if (state.row < state.visualLine.row) {
                    const newClipboard = ['\n'];
                    for (let i = state.row; i <= state.visualLine.row; i += 1) {
                        newClipboard.push(state.data[i]);
                    }
                    copyToClipboard(state, newClipboard);
                    state.data.splice(state.row, state.visualLine.row - state.row + 1);
                }
                const indentLevel = state.data[state.row] !== undefined ? getIndentLevelFrom(state, state.row) : 0;
                state.data.splice(state.row, 0, ' '.repeat(indentLevel));
                state.col = indentLevel;
                state.mode = 'i';
            }
        } else if (key === 'd') {
            if (state.row >= state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = ['\n'];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard);
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
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
            if (state.visualLine.row < startingPoint) {
                for (let i = state.visualLine.row; i <= startingPoint; i += 1) {
                    sendKeys(state.macro, state, screen);
                    up(state);
                }
            } else {
                for (let i = startingPoint; i <= state.visualLine.row; i += 1) {
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
        } else if (key === '=') {
            if (state.row >= state.visualLine.row) {
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    let indentLevel = i - 1 < 0 ? 0 : getIndentLevelFrom(state, i - 1);
                    if (state.data[i].trim().startsWith(')') || state.data[i].trim().startsWith('}')) {
                        indentLevel = indentLevel - 4 >= 0 ? indentLevel - 4 : 0;
                    }
                    state.data[i] = ' '.repeat(indentLevel) + state.data[i].trim();
                }
            } else if (state.row < state.visualLine.row) {
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    let indentLevel = i - 1 < 0 ? 0 : getIndentLevelFrom(state, i - 1);
                    if (state.data[i].trim().startsWith(')') || state.data[i].trim().startsWith('}')) {
                        indentLevel = indentLevel - 4 >= 0 ? indentLevel - 4 : 0;
                    }
                    state.data[i] = ' '.repeat(indentLevel) + state.data[i].trim();
                }
            }
            state.mode = 'n';
        } else if (key === '<') {
            if (state.row >= state.visualLine.row) {
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    decreaseIndentLevel(state, i);
                }
            } else if (state.row < state.visualLine.row) {
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    decreaseIndentLevel(state, i);
                }
            }
            state.col = firstNonSpace(state, state.row);
            state.row = state.visualLine.row;
            state.mode = 'n';
            createSnapshot(state);
        } else if (key === '>') {
            if (state.row >= state.visualLine.row) {
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    increaseIndentLevel(state, i);
                }
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    increaseIndentLevel(state, i);
                }
            }
            state.col = firstNonSpace(state, state.row);
            state.mode = 'n';
            createSnapshot(state);
        } else if (key === 'x') {
            if (state.row >= state.visualLine.row) {
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
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
            let systemPaste = ncp.paste();
            if (systemPaste.startsWith('\n')) {
                systemPaste = systemPaste.substring(1);
            }
            systemPaste = systemPaste.split('\n');
            if (systemPaste.length > 0) {
                if (state.row >= state.visualLine.row) {
                    state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                    state.row = state.visualLine.row;
                } else if (state.row < state.visualLine.row) {
                    state.data.splice(state.row, state.visualLine.row - state.row + 1);
                }
                for (let i = systemPaste.length - 1; i >= 0; i -= 1) {
                    state.data.splice(state.row, 0, systemPaste[i]);
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
        } else if (key === '(') {
            previousLowerIndentLevel(state, state.row);
            renderScreen(state, screen);
        } else if (key === ')') {
            nextLowerIndentLevel(state, state.row);
            renderScreen(state, screen);
        } else if (key === 'ESCAPE') {
            state.mode = 'n';
        }
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleVisualLineKeys
};
