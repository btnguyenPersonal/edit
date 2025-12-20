## tasks
- editing new file indicator [NEW] on right side??

- W and B

- have emacs find alt f
	- just need tab complete now

- ts-server integration (do it for real this time)

- start using namespace std

- localdefs ifdef TESTING last resort

- object oriented seam
    - consider changing some things to objects in order to test this way (supposed to be the best way)

- be careful when using linking seams, outside the environment is where the seam originates (hard to tell test vs prod)

- mocking
    - break clipboard dependency
    - break filesystem dependency
        - remove all the std::filesystem dependencies from everywhere and have replacement methods in filesystem.cpp
        - changes get sent to global testing variable, can read from that while testing
    - break ncurses dependency

- everything that we mock needs to have an integration test to cover the real-world application (macos, linux x11, linux wayland)

- each test should be <.1 s
- make sure there is a way to initialize State without needing to go to fs
- make describe beforeEach afterEach it
- test all of typingKeys

- if grep changes, put delay on for 5ms, and if it hasn't changed in that amount of time, spin it up. (hacky but would work for fuzzing)
- real solution would be changing every place where we get a file handle to have a mutex, and could make it a wrapper function
std::filesystem grabFile(State* state, std::string s);
void releaseFile(State* state, std::filesystem f);

- change state->file->data to rope structure
    - try to implement by hand that will be fun

- add crash dumps

# features
- repeat number keys
