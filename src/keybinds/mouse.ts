import * as helper from '../util/helper.ts';

function handleMouseInputs(name, coor, state, screen) {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        if (state.data[coor.y - 1 + (state.windowLine - 1)] && (coor.x - 1) - 5 >= 0) {
            if ((coor.x - 1) - 5 < state.data[coor.y - 1 + (state.windowLine - 1)].length) {
                state.col = (coor.x - 1) - 5;
            } else {
                state.col = state.data[coor.y - 1 + (state.windowLine - 1)].length;
            }
        } else {
            state.col = 0;
        }
        state.row = (coor.y - 1) + (state.windowLine - 1) < state.data.length
            ? (coor.y - 1) + (state.windowLine - 1)
            : state.data.length - 1;
        if (state.row === -1) {
            state.row = 0;
        }
        helper.renderScreen(state, screen);
    } else if (name === 'MOUSE_WHEEL_UP') {
        if (state.windowLine > 0) {
            state.windowLine -= 1;
            helper.renderScreen(state, screen, true);
        }
    } else if (name === 'MOUSE_WHEEL_DOWN') {
        if (state.windowLine + process.stdout.rows <= state.data.length) {
            state.windowLine += 1;
            helper.renderScreen(state, screen, true);
        }
    }
}

export {
    handleMouseInputs
};
