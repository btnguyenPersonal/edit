if (c == 'J') {
    state->col = state->data[state->row].length();
    if (state->row + 1 < state->data.size()) {
        ltrim(state->data[state->row + 1]);
        state->data[state->row] += state->data[state->row + 1];
        state->data.erase(state->data.begin() + state->row + 1);
    }
    state->dotCommand = c;
}