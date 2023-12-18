import {
    BLACK,
    GREY,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
} from './color.js';
import {
    COMMAND,
    FILEEXPLORER,
    FILEFINDER,
    GREP,
    HISTORY,
    TYPING,
    VISUALLINE,
    VISUAL,
    VISUALBLOCK,
} from './modes.js';
import {
    getCurrentWord,
    autocomplete,
    getCommentString,
    isOnScreen,
    centerScreen,
} from './helper.js';

function getRowIfOverflow(state) {
    return state.row < state.data.length
        ? state.row
        : state.data.length - 1;
}

function isHighlighted(state, i, j) {
    if (state.mode === VISUALLINE) {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else if (state.mode === VISUALBLOCK) {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            if (state.visual.col <= state.col) {
                if (j >= state.visual.col && j <= state.col) {
                    return true;
                }
            } else if (state.visual.col > state.col) {
                if (j >= state.col && j <= state.visual.col) {
                    return true;
                }
            }
        }
        return false;
    } else if (state.mode === VISUAL) {
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i < state.row && i > state.visual.row) || (i > state.row && i < state.visual.row)) {
            return true;
        } else {
            if (i === state.row && i === state.visual.row) {
                if (state.visual.col <= state.col) {
                    if (j >= state.visual.col && j <= state.col) {
                        return true;
                    }
                } else if (state.visual.col > state.col) {
                    if (j >= state.col && j <= state.visual.col) {
                        return true;
                    }
                }
            } else if (i === state.visual.row) {
                if (j >= state.visual.col && state.row > state.visual.row) {
                    return true;
                } else if (j <= state.visual.col && state.row < state.visual.row) {
                    return true;
                }
            } else if (i === state.row) {
                if (j <= state.col && state.row > state.visual.row) {
                    return true;
                } else if (j >= state.col && state.row < state.visual.row) {
                    return true;
                }
            }
        }
    }
    return false;
}

function moveCursor(state, screen, windowLineHorizontal) {
    if (state.mode === COMMAND) {
        screen.moveTo(
            state.commandCursorPosition + 1,
            0
        );
    } else {
        const r = getRowIfOverflow(state);
        let row;
        let col;
        if (state.data[r] !== undefined) {
            col = state.col < state.data[r].length
                ? state.col
                : state.data[r].length;
        } else {
            col = 0;
        }
        col += 5;
        row = state.row < state.data.length ? state.row : state.data.length;
        col -= windowLineHorizontal;
        row -= state.windowLine;
        screen.moveTo(
            col,
            row + 1
        );
    }
}

function isRowHighlighted(state, i) {
    if (state.mode === VISUALLINE) {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else if (state.mode === VISUALBLOCK) {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else if (state.mode === VISUAL) {
        if ((i <= state.row && i >= state.visual.row) || (i >= state.row && i <= state.visual.row)) {
            return true;
        }
    } else {
        return false;
    }
}

function shortenFilePath(filePath) {
    const parts = filePath.split('/');
    for (let i = 0; i < parts.length - 1; i += 1) {
        if (parts[i].length > 1) {
            parts[i] = parts[i][0];
        }
    }
    return parts.join('/');
}

function isMergeConflictStart(s) {
    return s.startsWith('<<<<<<<');
}

function isMergeConflictMiddle(s) {
    return s.startsWith('=======');
}

function isMergeConflictEnd(s) {
    return s.startsWith('>>>>>>>');
}

function commentStartsAt(state, row) {
    const commentString = getCommentString(state.file);
    let inString = false;
    let disregardNext = false;
    let stringChar;
    if (commentString === '') {
        return -1;
    }
    for (let i = 0; i < state.data[row].length; i += 1) {
        const s = state.data[row].substring(i, i + 1);
        if (s === stringChar && stringChar !== undefined && !disregardNext) {
            inString = !inString;
            stringChar = undefined;
        } else if (s === '"' || s === '\'' || s === '`') {
            if (stringChar === undefined) {
                inString = !inString;
                stringChar = s;
            }
        }
        if (state.data[row].substring(i, i + commentString.length) === commentString && !inString) {
            return i;
        }
        if (inString && s === '\\' && !disregardNext) {
            disregardNext = true;
        } else {
            disregardNext = false;
        }
    }
    return -1;
}

function calcIsInString(
    color,
    s,
    stringChar,
    disregardNext,
    commentString,
    inString
) {
    if (s === stringChar && stringChar !== undefined && !disregardNext) {
        inString = !inString;
        stringChar = undefined;
    } else if (s === '"' || s === '\'' || s === '`') {
        color = CYAN;
        if (stringChar === undefined && commentString !== '' && commentString !== '<!--') {
            inString = !inString;
            stringChar = s;
        }
    }
    return [color, inString, stringChar];
}

function getColorRow(state, row, displayRow, commentIndex, isCurrentRow, commentString) {
    const output = [];
    let inString = false;
    let stringChar;
    let color = WHITE;
    let disregardNext = false;
    let counter = 0;
    let startOfSearch;
    let nonDisplayRowIndex = 0;
    for (let i = 0; i < displayRow.length; i += 1) {
        if (i > 160) {
            output.push(RED);
        } else {
            const s = displayRow.substring(i, i + 1);
            if (inString) {
                color = CYAN;
            }
            [color, inString, stringChar] = calcIsInString(color, s, stringChar, disregardNext, commentString, inString);
            if (counter !== 0 || (
                state.searching && state.searchQuery.length !== 0
                && state.searchQuery === displayRow.substring(i, i + state.searchQuery.length)
            )) {
                if (counter === 0) {
                    startOfSearch = i;
                    counter = state.searchQuery.length;
                    nonDisplayRowIndex += state.searchQuery.length;
                }
                if (!state.replacing && isCurrentRow && startOfSearch + state.searchQuery.length >= state.col && startOfSearch <= state.col) {
                    output.push('searchCurrent');
                } else {
                    output.push('search');
                }
            } else if (counter !== 0 || (
                state.replacing && state.replaceQuery.length !== 0
                && state.replaceQuery === displayRow.substring(i, i + state.replaceQuery.length)
                && state.searchQuery === row.substring(nonDisplayRowIndex, nonDisplayRowIndex + state.searchQuery.length)
            )) {
                if (counter === 0) {
                    counter = state.replaceQuery.length;
                    nonDisplayRowIndex += state.searchQuery.length;
                }
                output.push('search');
            } else if (!inString && (s === '(' || s === ')')) {
                color = YELLOW;
            } else if (!inString && (s === '[' || s === ']')) {
                color = GREEN;
            } else if (!inString && (s === '{' || s === '}')) {
                color = MAGENTA;
            }
            if (i >= commentIndex && commentIndex !== -1) {
                color = GREEN;
            }
            if (counter === 0) {
                output.push(color);
            }
            if (inString && s === '\\' && !disregardNext) {
                disregardNext = true;
            } else {
                disregardNext = false;
            }
            color = WHITE;
            if (counter > 0) {
                counter -= 1;
            } else {
                nonDisplayRowIndex += 1;
            }
        }
    }
    return output;
}

function renderSingleLine(state, screen, i, mergeSection, isContext) {
    let section = mergeSection;
    let numColor = GREY;
    let numBgColor = BLACK;
    if (state.recording) {
        numColor = RED;
    } else if (state.row === i) {
        numColor = WHITE;
    }
    if (i === state.mark) {
        numBgColor = GREEN;
        numColor = BLACK;
    } else if (i === state.mark2) {
        numBgColor = CYAN;
        numColor = BLACK;
    }
    screen.put({
        attr: { color: numColor, bgColor: numBgColor, underline: isContext },
        x: 0
    }, (i + 1).toString().padStart(4) + ' ');
    if (isMergeConflictStart(state.data[i])) {
        section = 1;
    } else if (isMergeConflictMiddle(state.data[i])) {
        section = 2;
    } else if (isMergeConflictEnd(state.data[i])) {
        section = 0;
    }
    let displayRow = state.data[i];
    if (state.replacing) {
        displayRow = displayRow.replaceAll(state.searchQuery, state.replaceQuery);
    }
    const colorRow = getColorRow(
        state,
        state.data[i],
        displayRow,
        commentStartsAt(state, i),
        state.row === i,
        getCommentString(state.file)
    );
    for (let j = state.windowLineHorizontal; j < displayRow.length; j += 1) {
        let color = colorRow[j];
        let bgColor = BLACK;
        if (section === 1 && !isMergeConflictStart(displayRow)) {
            color = RED;
        } else if (section === 2 && !isMergeConflictMiddle(displayRow)) {
            color = GREEN;
        } else if (isMergeConflictEnd(displayRow) || isMergeConflictMiddle(displayRow) || isMergeConflictStart(displayRow)) {
            color = BLUE;
        }
        if (colorRow[j] === 'search') {
            if (state.replacing || state.searching) {
                color = BLACK;
                bgColor = CYAN;
            }
        } else if (colorRow[j] === 'searchCurrent') {
            color = BLACK;
            bgColor = MAGENTA;
        }
        screen.put({
            attr: {
                color,
                bgColor,
                inverse: isHighlighted(state, i, j),
                underline: isContext
            },
            wrap: false
        }, displayRow.substring(j, j + 1));
        if (state.row === i && state.col - 1 === j && state.mode === TYPING) {
            const str = getCurrentWord(state.data[state.row], state.col);
            const replaceString = autocomplete(str, state.data);
            if (str.length !== 0) {
                screen.put({
                    attr: {
                        color: 'gray',
                        bgColor: BLACK
                    },
                    wrap: false
                }, replaceString.substring(str.length));
            }
        }
    }
    if (state.data[i] === '' && isRowHighlighted(state, i)) {
        screen.put({
            attr: {
                inverse: true
            },
            wrap: false
        }, ' ');
    }
    screen.put({ newLine: true }, '\n');
    return section;
}

function getWindowLineHorizontal(state) {
    let col;
    if (state.data[state.row] !== undefined) {
        col = state.col < state.data[state.row].length
            ? state.col
            : state.data[state.row].length;
    } else {
        col = 0;
    }
    if (col - state.windowLineHorizontal > process.stdout.columns - 6) {
        state.windowLineHorizontal = (col - process.stdout.columns + 6) + 10;
    } else if (col - state.windowLineHorizontal < 0) {
        state.windowLineHorizontal = col - 10 > 0 ? col - 10 : 0;
    }
}

function renderWindowLines(state, screen, noCenterScreen, fullRefresh) {
    if (!noCenterScreen && !isOnScreen(state)) {
        centerScreen(state);
    }
    getWindowLineHorizontal(state);
    let mergeSection = 0;
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows) - 1; i += 1) {
        if (state.data[i] !== undefined) {
            mergeSection = renderSingleLine(state, screen, i, mergeSection);
        }
    }
    moveCursor(state, screen, state.windowLineHorizontal);
    screen.draw({ delta: !fullRefresh });
    screen.drawCursor();
}

function renderHistoryTree(state, screen) {
    const index = state.fileFinderIndex - Math.floor(process.stdout.rows / 2) > 0 ? state.fileFinderIndex - Math.floor(process.stdout.rows / 2) : 0;
    for (let i = index; i < state.fileFinderOutput.length && i < index + process.stdout.rows - 2; i += 1) {
        screen.put({
            attr: {
                color: state.fileFinderIndex === i ? RED : WHITE,
                bgColor: BLACK,
            },
            x: 0,
            wrap: false
        }, state.fileFinderOutput[i]);
        screen.put({ newLine: true }, '\n');
    }
    screen.moveTo(0, 0);
    screen.draw({ delta: true });
    screen.drawCursor();
}

function getFileFinderColor(mode) {
    return mode === GREP ? GREEN : YELLOW;
}

function getExplorerColor(state, i) {
    if (state.fileExplorerOutput[i].includes('__DIR')) {
        return BLUE;
    } else if (state.selectedFileExplorerIndex !== -1 && state.selectedFileExplorerIndex === i) {
        return YELLOW;
    } else {
        return WHITE;
    }
}

function renderFileFinder(state, screen, mode) {
    const query = mode === GREP ? state.grepQuery : state.fileFinderQuery;
    const modeIndex = mode === GREP ? state.grepIndex : state.fileFinderIndex;
    const curPos = mode === GREP ? state.grepCursorPosition : state.fileFinderCursorPosition;
    screen.put({
        attr: {
            color: getFileFinderColor(mode),
            bgColor: BLACK,
        },
        x: 0,
        wrap: false
    }, '> ');
    screen.put({
        attr: {
            color: WHITE,
            bgColor: BLACK,
        },
        wrap: false
    }, query);
    const index = modeIndex - Math.floor(process.stdout.rows / 2) > 0 ? modeIndex - Math.floor(process.stdout.rows / 2) : 0;
    for (let i = index; i < state.fileFinderOutput.length && i < index + process.stdout.rows - 2; i += 1) {
        screen.put({ newLine: true }, '\n');
        screen.put({
            attr: {
                color: WHITE,
                bgColor: BLACK,
                inverse: modeIndex === i
            },
            x: 0,
            wrap: false
        }, state.fileFinderOutput[i]);
    }
    screen.moveTo(
        curPos + 2,
        1
    );
    screen.draw({ delta: true });
    screen.drawCursor();
}

function renderFileExplorer(state, screen) {
    const index = state.fileExplorerIndex - Math.floor(process.stdout.rows / 2) > 0 ? state.fileExplorerIndex - Math.floor(process.stdout.rows / 2) : 0;
    for (let i = index; i < state.fileExplorerOutput.length && i < index + process.stdout.rows - 2; i += 1) {
        screen.put({
            attr: {
                color: getExplorerColor(state, i),
                bgColor: BLACK,
                inverse: state.fileExplorerIndex === i,
            },
            x: 0,
            wrap: false,
        }, state.fileExplorerOutput[i].replace(/__DIR/, ''));
        screen.put({ newLine: true }, '\n');
    }
    screen.moveTo(state.newFileIndex + 2, 0);
    screen.draw({ delta: true });
    screen.drawCursor();
}

function renderStatusBar(state, screen) {
    if (state.status !== '') {
        screen.put({ attr: { color: RED, bgColor: BLACK } }, state.status + ' ');
    }
    if (state.mode === FILEEXPLORER) {
        screen.put({ attr: { color: state.typing ? WHITE : GREY, bgColor: BLACK } }, '> ' + state.newFile);
    } else if (state.mode === COMMAND) {
        screen.put({ attr: { color: WHITE, bgColor: BLACK } }, ':' + state.currentCommand);
    } else {
        screen.put({ attr: { color: GREEN, bgColor: BLACK } }, '/' + state.searchQuery + ' ');
        for (let i = 0; i < state.harpoonIndexes.length; i += 1) {
            screen.put({
                attr: {
                    color: i === state.harpoonIndex && state.file === state.files[state.harpoonIndexes[i]] ? YELLOW : GREY,
                    bgColor: BLACK
                }
            }, shortenFilePath(state.files[state.harpoonIndexes[i]]) + ' ');
        }
    }
    screen.put({ attr: { color: WHITE, bgColor: BLACK }, x: process.stdout.columns - (2 + state.file.length) }, '"' + state.file + '"');
}

function setHarpoonIndex(state) {
    if (state.harpoonIndexes.includes(state.fileIndex)) {
        state.harpoonIndex = state.harpoonIndexes.indexOf(state.fileIndex);
    }
}

function render(state, screen, noCenterScreen, fullRefresh) {
    if (state.data.length === 0) {
        state.data = [''];
    }
    screen.fill({
        attr: { bgColor: BLACK }
    });
    screen.moveTo(0, 0);
    setHarpoonIndex(state);
    renderStatusBar(state, screen);
    screen.put({ newLine: true }, '\n');
    if (state.mode === HISTORY) {
        renderHistoryTree(state, screen);
    } else if (state.mode === GREP || state.mode === FILEFINDER) {
        renderFileFinder(state, screen, state.mode);
    } else if (state.mode === FILEEXPLORER) {
        renderFileExplorer(state, screen);
    } else if (state.allowCommandLogging) {
        renderWindowLines(state, screen, noCenterScreen, fullRefresh);
    }
}

export {
    render
};
