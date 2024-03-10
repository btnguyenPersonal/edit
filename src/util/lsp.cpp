#include "state.h"
#include "lsp.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>

bool isLanguageServerInstalled() {
    int result = system("typescript-language-server --version 2> /dev/null");
    return result == 0;
}

FILE* startLanguageServer() {
    FILE* lspProcess = popen("typescript-language-server --stdio", "r");
    if (!lspProcess) {
        return nullptr;
    }
    return lspProcess;
}

void sendLSPInitRequest(State* state) {
    std::string initRequest = R"({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {"processId": null, "rootUri": null, "capabilities": {}}})";
    initRequest += "\n";
    fwrite(initRequest.c_str(), 1, initRequest.size(), state->lspProcess);
    fflush(state->lspProcess);
}

void readLSPResponse(State* state) {
    char buffer[4096];
    if (fgets(buffer, sizeof(buffer), state->lspProcess) != NULL) {
        std::cout << "Received response: " << buffer << std::endl;
    } else {
        std::cerr << "Failed to read response from language server." << std::endl;
    }
}
