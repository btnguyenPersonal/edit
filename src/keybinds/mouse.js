import * as helper from '../util/helper.js';

function handleMouseInputs(name, coor, state, term) {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        state.col = (coor.x - 1) - 4 >= 0 ? (coor.x - 1) - 4 : 0;
        state.row = (coor.y - 1) + state.windowLine < state.data.length
            ? (coor.y - 1) + state.windowLine
            : state.data.length - 1;
        helper.moveCursor(state, term);
    } else if (name === 'MOUSE_WHEEL_DOWN') {
        if (state.windowLine > 0) {
            state.windowLine -= 1;
            helper.renderScreen(state, term);
        }
    } else if (name === 'MOUSE_WHEEL_UP') {
        if (state.windowLine + process.stdout.rows < state.data.length) {
            state.windowLine += 1;
            helper.renderScreen(state, term);
        }
    }
}

export {
    handleMouseInputs
};