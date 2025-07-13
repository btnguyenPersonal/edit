fix off-screen

## tasks
- add crash reports
```
    std::filesystem::path home = std::filesystem::absolute(getenv("HOME"));
    state->print(home.string() + "/.crashreport");
```
- fix expand file not there anymore
- inspect memory leaks using new
```
    Object * object1 = new Object(); //A new object is allocated on the heap
    Object * object2 = new Object(); //Another new object is allocated on the heap
    delete object1;

    // Since C++ does not have a garbage collector, if we don't do that, the next line would
    // cause a "memory leak", i.e. a piece of claimed memory that the app cannot use
    // and that we have no way to reclaim...
    object1 = object2; //Same as Java, object1 points to object2.
```
- fix issue with going up not saving col (tabs counting as 1 instead of 8)
- better replace all in project (interactive mode and better command mode)
- make indentation work not on type, but always, and just trim extra newlines when whole row is empty on escape of typing
	- needs to also have enterInsertMode function that puts spaces/tabs in front
- retab auto-detect indentation
- make gs///g respect ignoreFiles
- make b work with "../"
- fileExplorer issues
- fix tab rendering with non wordwrap state->windowPosition.col > 0
- lock down changing files flag
  - turn on when unsaved??
- look at how hard it would be to have an icon at top to say there are new changes available?
  - each action check? if new changes happen have a popup like vim??
- add args
  - turn off autosave cmd
  -c "commands" (need to make parser for <c-h> <cr> etc)
  --help
  -h
- fix editorconfig filename parser
- speed up grep to as fast as rg with their wizard magic
- fix diff function for huge files (look at vim, rope, other stuff)
  - futures api probably bad, use <pthread> semaphores and locks
  - do this with tests!!
  - have specific timestamps, and make a logging function that shows what happened when (with locks)
