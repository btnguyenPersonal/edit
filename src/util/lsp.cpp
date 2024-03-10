#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>

bool isLanguageServerInstalled() {
    int result = system("typescript-language-server --version > nul 2>&1");
    return result == 0;
}

FILE* startLanguageServer() {
    const char* cmd = "typescript-language-server --stdio";
    FILE* lspProcess = popen(cmd, "r+");
    if (!lspProcess) {
        return nullptr;
    }
    return lspProcess;
}
