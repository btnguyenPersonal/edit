#include "../util/state.h"
#include "../util/keys.h"
#include "../keybinds/mainLoop.h"
#include <ncurses.h>
#include <random>
#include <vector>

void fuzzSendKeys(int testnum, int iterations = 1000)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::vector<char> keypresses = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', '\b', '\n', '\t', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x7F', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '\'', '"', '`', '~', ';', ':', '[', ']', '{', '}', ',', '<', '.', '>', '/', '?', '|', '\\', '-', '_', '+', '=' };

	std::uniform_int_distribution<> dis(0, keypresses.size() - 1);
	State *state = new State("./test-file.h");

	try {
		std::vector<char> randVec;
		for (int i = 0; i < iterations; i++) {
			randVec.push_back(keypresses[dis(gen)]);
		}
		printf("\tState *state = new State(\"./test-file.h\");\n");
		printf("\ttestValues(state, {");
		for (uint32_t i = 0; i < randVec.size(); i++) {
			if (i != 0) {
				printf(", ");
			}
			std::string c = getEscapedChar(randVec[i], true);
			std::string s = "";
			for (uint32_t j = 0; j < c.length(); j++) {
				if (c[j] == '\\' || c[j] == '"') {
					s += "\\";
				}
				s += c[j];
			}
			printf("\"%s\"", s.c_str());
		}
		printf("});\n");
		for (uint32_t i = 0; i < randVec.size(); i++) {
			char key = randVec[i];
			printf("%s %s\n", getMode(state->mode).c_str(), getEscapedChar(key, true).c_str());
			mainLoop(state, ERR);
			mainLoop(state, ERR);
			mainLoop(state, ERR);
			mainLoop(state, ERR);
			mainLoop(state, key);
		}
	} catch (const std::exception &e) {
		printf("Error during fuzzing: %s\n", e.what());
		exit(1);
	}
}

void testValues(State *state, std::vector<std::string> v)
{
	for (uint32_t i = 0; i < v.size(); i++) {
		char key = getUnEscapedChar(v[i]);
		printf("%s %s\n", getMode(state->mode).c_str(), getEscapedChar(key, true).c_str());
		mainLoop(state, key);
	}
}

int main()
{
	// State *state = new State("./test-file.h");
	// testValues(state, {"E", "N", "<C-O>", ">", ">", "l", "<C-^>", " \\\\", "3", "<C-P>"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"r", "~", "Q", ">", "V", "I", "p", "<C-A>", "<C-K>", "<C-@>"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"{", "~", "5", "r", "<Space>", "<C-O>", "<C-_>", "d", "G", "<C-]>"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"|", "<C-A>", "'", "<C-U>", "U", "#", "7", "T", ":", "l"});
	// State *state = new State("./test-file.h");
	// testValues(state, {")", "n", "S", "b", "#", "<C-_>", ";", "c", "(", "<C-Q>"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"%", "[", "'", "+", "<C-E>", "}", "=", "<C-P>", "3", "X"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"M", "K", "p", "J", "]", "]", "T", "A", "<C-J>", "k"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"H", "<C-I>", "/", "<C-Q>", "'", "<C-H>", "3", "w", "2", "#"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"<C-G>", "Z", "<C-O>", "<C-P>", "p", "<C-@>", "1", "<C-J>", "M", "5"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"z", "v", "<C-@>", "<C-H>", "T", "*", ".", "2", "=", "."});
	// State *state = new State("./test-file.h");
	// testValues(state, {"V", "=", ".", "<C-P>", "<C-]>", "U", "<Esc>", "Y", "h", "p"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"l", "<C-I>", "#", "h", "<C-I>", "<C-V>", "o", "<C-J>", "|", "d"});
	// State *state = new State("./test-file.h");
	// testValues(state, {"<C-E>", "8", "<C-G>", "G", "b", "g", "J", "u", "u", "<C-L>"});
	/*
	State *state = new State("./test-file.h");
	testValues(state, {"<C-G>", "E", "<C-V>", "<C-I>", "G", "<C-J>", "<C-O>", "$", "<C-M>", "."});
NORMAL <C-G>
GREP E
GREP <C-V>
GREP <C-I>
GREP G
GREP <C-J>
==159893==ERROR: ThreadSanitizer: requested allocation size 0x64652f7469672f6f exceeds maximum supported size of 0x10000000000
    #0 <null> <null> (libtsan.so.2+0xaf080) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 <null> <null> (fuzz+0x1542e) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #2 <null> <null> (fuzz+0x977b5) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #3 <null> <null> (fuzz+0x62773) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #4 <null> <null> (fuzz+0xb7fd0) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #5 <null> <null> (fuzz+0xb88ca) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #6 <null> <null> (fuzz+0xadb89) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #7 <null> <null> (fuzz+0x13d71) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #8 <null> <null> (fuzz+0x12bbb) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
    #9 <null> <null> (libc.so.6+0x27634) (BuildId: 2f722da304c0a508c891285e6840199c35019c8d)

==159893==HINT: if you don't care about these errors you may set allocator_may_return_null=1
SUMMARY: ThreadSanitizer: allocation-size-too-big (/home/ben/git/edit/build/fuzz+0x1542e) (BuildId: 223a085c954ca3ee97e83852c27b411f6f40aee0)
make: *** [Makefile:95: fuzz] Error 66
*/

/*
	State *state = new State("./test-file.h");
	testValues(state, {"'", "<C-Q>", "G", "Y", "<C-M>", "?", "<C-J>", "S", "9", "L"});
NORMAL '
NORMAL <C-Q>
DIFF G
DIFF Y
DIFF <C-M>
DIFF ?
DIFF <C-J>
Error during fuzzing: basic_string::_M_create
make: *** [Makefile:95: fuzz] Error 1
*/
/*
	State *state = new State("./test-file.h");
	testValues(state, {"8", "<C-G>", "[", "<C-H>", "g", "<C-J>", "<C-_>", "<C-T>", "<C-I>", "'"});
NORMAL 8
NORMAL <C-G>
GREP [
GREP <C-H>
GREP g
GREP <C-J>
==================
WARNING: ThreadSanitizer: heap-use-after-free (pid=464533)
  Read of size 8 at 0x7208009c5048 by main thread (mutexes: write M0):
    #0 getDisplayRows(State*, unsigned int) <null> (fuzz+0x27bd7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #1 getCenteredWindowPosition(State*) <null> (fuzz+0x28056) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 centerScreen(State*) <null> (fuzz+0x85cf7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 sendGrepKeys(State*, int) <null> (fuzz+0xb8bab) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 sendKeys(State*, int) <null> (fuzz+0xb948a) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 mainLoop(State*, int) <null> (fuzz+0xae749) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #6 fuzzSendKeys(int, int) <null> (fuzz+0x13e81) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #7 main <null> (fuzz+0x12ccb) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)

  Previous write of size 8 at 0x7208009c5048 by thread T19431:
    #0 operator delete(void*, unsigned long) /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_new_delete.cpp:150 (libtsan.so.2+0xae867) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 grepFile(std::filesystem::__cxx11::path const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::filesystem::__cxx11::path const&, std::mutex&, std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > >&) <null> (fuzz+0x642d0) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 grepThread(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::filesystem::__cxx11::path const&, std::mutex&, std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > >&, std::mutex&, std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> >&) <null> (fuzz+0x648e7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::filesystem::__cxx11::path const&, std::mutex&, std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > >&, std::mutex&, std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> >&), std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::filesystem::__cxx11::path, std::reference_wrapper<std::mutex>, std::reference_wrapper<std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > > >, std::reference_wrapper<std::mutex>, std::reference_wrapper<std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> > > > > >::_M_run() <null> (fuzz+0x68d88) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 execute_native_thread_routine /usr/src/debug/gcc/gcc/libstdc++-v3/src/c++11/thread.cc:104 (libstdc++.so.6+0xe55a3) (BuildId: fd64159c3649883e78852adc134c87393690dba7)

  Mutex M0 (0x726400aca850) created at:
    #0 pthread_mutex_lock /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_interceptors_posix.cpp:1371 (libtsan.so.2+0x5f03c) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 sanityCheckGrepSelection(State*) <null> (fuzz+0x95b7f) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 cleanup(State*, char) <null> (fuzz+0x15b90) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 mainLoop(State*, int) <null> (fuzz+0xae757) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 fuzzSendKeys(int, int) <null> (fuzz+0x13e81) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 main <null> (fuzz+0x12ccb) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)

  Thread T19431 (tid=507216, finished) created by thread T19428 at:
    #0 pthread_create /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_interceptors_posix.cpp:1041 (libtsan.so.2+0x5fb47) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 __gthread_create(unsigned long*, void* (*)(void*), void*) /usr/src/debug/gcc/gcc-build/x86_64-pc-linux-gnu/libstdc++-v3/include/x86_64-pc-linux-gnu/bits/gthr-default.h:709 (libstdc++.so.6+0xe56a1) (BuildId: fd64159c3649883e78852adc134c87393690dba7)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) /usr/src/debug/gcc/gcc/libstdc++-v3/src/c++11/thread.cc:172 (libstdc++.so.6+0xe56a1)
    #3 grepDispatch(State*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool) <null> (fuzz+0x666be) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)(State*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool), State*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> > >::_M_run() <null> (fuzz+0x692db) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 execute_native_thread_routine /usr/src/debug/gcc/gcc/libstdc++-v3/src/c++11/thread.cc:104 (libstdc++.so.6+0xe55a3) (BuildId: fd64159c3649883e78852adc134c87393690dba7)

SUMMARY: ThreadSanitizer: heap-use-after-free (/home/ben/git/edit/build/fuzz+0x27bd7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9) in getDisplayRows(State*, unsigned int)
==================
==================
WARNING: ThreadSanitizer: heap-use-after-free (pid=464533)
  Read of size 8 at 0x7208009c5040 by main thread (mutexes: write M0):
    #0 getDisplayRows(State*, unsigned int) <null> (fuzz+0x27be3) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #1 getCenteredWindowPosition(State*) <null> (fuzz+0x28056) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 centerScreen(State*) <null> (fuzz+0x85cf7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 sendGrepKeys(State*, int) <null> (fuzz+0xb8bab) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 sendKeys(State*, int) <null> (fuzz+0xb948a) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 mainLoop(State*, int) <null> (fuzz+0xae749) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #6 fuzzSendKeys(int, int) <null> (fuzz+0x13e81) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #7 main <null> (fuzz+0x12ccb) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)

  Previous write of size 8 at 0x7208009c5040 by thread T19431:
    #0 operator delete(void*, unsigned long) /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_new_delete.cpp:150 (libtsan.so.2+0xae867) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 grepFile(std::filesystem::__cxx11::path const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::filesystem::__cxx11::path const&, std::mutex&, std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > >&) <null> (fuzz+0x642d0) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 grepThread(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::filesystem::__cxx11::path const&, std::mutex&, std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > >&, std::mutex&, std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> >&) <null> (fuzz+0x648e7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::filesystem::__cxx11::path const&, std::mutex&, std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > >&, std::mutex&, std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> >&), std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::filesystem::__cxx11::path, std::reference_wrapper<std::mutex>, std::reference_wrapper<std::vector<std::vector<grepMatch, std::allocator<grepMatch> >, std::allocator<std::vector<grepMatch, std::allocator<grepMatch> > > > >, std::reference_wrapper<std::mutex>, std::reference_wrapper<std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> > > > > >::_M_run() <null> (fuzz+0x68d88) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 execute_native_thread_routine /usr/src/debug/gcc/gcc/libstdc++-v3/src/c++11/thread.cc:104 (libstdc++.so.6+0xe55a3) (BuildId: fd64159c3649883e78852adc134c87393690dba7)

  Mutex M0 (0x726400aca850) created at:
    #0 pthread_mutex_lock /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_interceptors_posix.cpp:1371 (libtsan.so.2+0x5f03c) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 sanityCheckGrepSelection(State*) <null> (fuzz+0x95b7f) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 cleanup(State*, char) <null> (fuzz+0x15b90) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 mainLoop(State*, int) <null> (fuzz+0xae757) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 fuzzSendKeys(int, int) <null> (fuzz+0x13e81) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 main <null> (fuzz+0x12ccb) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)

  Thread T19431 (tid=507216, finished) created by thread T19428 at:
    #0 pthread_create /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_interceptors_posix.cpp:1041 (libtsan.so.2+0x5fb47) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 __gthread_create(unsigned long*, void* (*)(void*), void*) /usr/src/debug/gcc/gcc-build/x86_64-pc-linux-gnu/libstdc++-v3/include/x86_64-pc-linux-gnu/bits/gthr-default.h:709 (libstdc++.so.6+0xe56a1) (BuildId: fd64159c3649883e78852adc134c87393690dba7)
    #2 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) /usr/src/debug/gcc/gcc/libstdc++-v3/src/c++11/thread.cc:172 (libstdc++.so.6+0xe56a1)
    #3 grepDispatch(State*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool) <null> (fuzz+0x666be) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)(State*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool), State*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> > >::_M_run() <null> (fuzz+0x692db) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 execute_native_thread_routine /usr/src/debug/gcc/gcc/libstdc++-v3/src/c++11/thread.cc:104 (libstdc++.so.6+0xe55a3) (BuildId: fd64159c3649883e78852adc134c87393690dba7)

SUMMARY: ThreadSanitizer: heap-use-after-free (/home/ben/git/edit/build/fuzz+0x27be3) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9) in getDisplayRows(State*, unsigned int)
==================
==464533==ERROR: ThreadSanitizer: requested allocation size 0x616f6270696c6330 exceeds maximum supported size of 0x10000000000
    #0 operator new(unsigned long) /usr/src/debug/gcc/gcc/libsanitizer/tsan/tsan_new_delete.cpp:64 (libtsan.so.2+0xaf080) (BuildId: 00f0860667bfad762ad5a445880562fa1a4575e6)
    #1 void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<true>(char const*, unsigned long) <null> (fuzz+0x1553e) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #2 getDisplayRows(State*, unsigned int) <null> (fuzz+0x27bf2) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #3 getCenteredWindowPosition(State*) <null> (fuzz+0x28056) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #4 centerScreen(State*) <null> (fuzz+0x85cf7) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #5 sendGrepKeys(State*, int) <null> (fuzz+0xb8bab) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #6 sendKeys(State*, int) <null> (fuzz+0xb948a) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #7 mainLoop(State*, int) <null> (fuzz+0xae749) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #8 fuzzSendKeys(int, int) <null> (fuzz+0x13e81) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #9 main <null> (fuzz+0x12ccb) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9)
    #10 <null> <null> (libc.so.6+0x27634) (BuildId: 2f722da304c0a508c891285e6840199c35019c8d)

==464533==HINT: if you don't care about these errors you may set allocator_may_return_null=1
SUMMARY: ThreadSanitizer: allocation-size-too-big (/home/ben/git/edit/build/fuzz+0x1553e) (BuildId: b19ca7aff5671dd429016ba27fb6f04b132315a9) in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<true>(char const*, unsigned long)
*/
	for (uint32_t i = 0; i < 1000000; i++) {
		fuzzSendKeys(i, 10);
	}
	return 0;
}
