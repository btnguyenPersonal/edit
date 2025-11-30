## tasks
- each test should be <.1 s
- make sure there is a way to initialize State without needing to go to fs
- make describe beforeEach afterEach it
- test all of typingKeys

- if grep changes, put delay on for 5ms, and if it hasn't changed in that amount of time, spin it up. (hacky but would work for fuzzing)
- real solution would be changing every place where we get a file handle to have a mutex, and could make it a wrapper function
std::filesystem grabFile(State* state, std::string s);
void releaseFile(State* state, std::filesystem f);

- change state->data to rope structure
    - try to implement by hand that will be fun

- add crash dumps

# features
- repeat keys
