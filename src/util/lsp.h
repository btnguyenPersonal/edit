#include "state.h"

bool isLanguageServerInstalled();
FILE* startLanguageServer();
void sendLSPInitRequest(State* state);
void readLSPResponse(State* state);
