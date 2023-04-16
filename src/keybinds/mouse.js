import * as helper from '../util/helper.js';

function handleMouseInputs(name, coor, state, screen) {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        if ((coor.x - 1) - 5 >= 0) {
            if ((coor.x - 1) - 5 < state.data[coor.y - 1 + state.windowLine].length) {
                state.col = (coor.x - 1) - 5;
            } else {
                state.col = state.data[coor.y - 1].length;
            }
        } else {
            state.col = 0;
        }
        state.row = (coor.y - 1) + state.windowLine < state.data.length
            ? (coor.y - 1) + state.windowLine
            : state.data.length - 1;
        helper.renderScreen(state, screen);
    } else if (name === 'MOUSE_WHEEL_UP') {
        if (state.windowLine > 0) {
            state.windowLine -= 1;
            helper.renderScreen(state, screen);
        }
    } else if (name === 'MOUSE_WHEEL_DOWN') {
        if (state.windowLine + process.stdout.rows < state.data.length) {
            state.windowLine += 1;
            helper.renderScreen(state, screen);
        }
    }
}

export {
    handleMouseInputs
};
