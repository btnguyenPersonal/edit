#include "tsServer.h"
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <signal.h>
#include <string>
#include <memory>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "../util/state.h"
#include "../util/clipboard.h"

void tsHelloWorld(State *state)
{
	// Create a temporary file for LSP communication
	std::string tmpIn = "/tmp/lsp_in_" + std::to_string(getpid());
	std::string tmpOut = "/tmp/lsp_out_" + std::to_string(getpid());

	// Create named pipes
	if (mkfifo(tmpIn.c_str(), 0666) != 0 || mkfifo(tmpOut.c_str(), 0666) != 0) {
		state->status = "Failed to create pipes";
		return;
	}

	// Fork to start the TypeScript language server
	pid_t serverPid = fork();
	if (serverPid == 0) {
		// Child process: start the language server
		int inFd = open(tmpIn.c_str(), O_RDONLY | O_NONBLOCK);
		int outFd = open(tmpOut.c_str(), O_WRONLY | O_NONBLOCK);

		dup2(inFd, STDIN_FILENO);
		dup2(outFd, STDOUT_FILENO);
		dup2(outFd, STDERR_FILENO);

		execlp("typescript-language-server", "typescript-language-server", "--stdio", "--log-level", "4", nullptr);
		_exit(1); // If execlp fails
	} else if (serverPid > 0) {
		// Parent process: wait a bit for server to start
		usleep(500000); // 0.5 second

		// Send initialize request
		nlohmann::json request;
		request["jsonrpc"] = "2.0";
		request["id"] = 1;
		request["method"] = "initialize";

		nlohmann::json params;
		params["processId"] = getpid();
		params["rootUri"] = "file://" + std::filesystem::current_path().string();

		nlohmann::json capabilities;
		nlohmann::json textDocument;
		nlohmann::json completion;
		nlohmann::json completionItem;
		completionItem["snippetSupport"] = true;
		completion["completionItem"] = completionItem;
		textDocument["completion"] = completion;
		capabilities["textDocument"] = textDocument;

		params["capabilities"] = capabilities;
		request["params"] = params;

		std::string requestStr = request.dump();
		std::string lspMessage = "Content-Length: " + std::to_string(requestStr.length()) + "\r\n\r\n" + requestStr;

		// Write request to server (write to tmpIn, server reads from this)
		std::ofstream serverInPipe(tmpIn);
		serverInPipe << lspMessage;
		serverInPipe.flush();
		serverInPipe.close();

		// Read response from server (read from tmpOut, server writes to this)
		usleep(1000000); // 1 second for response

		std::ifstream serverOutPipe(tmpOut);
		std::string response((std::istreambuf_iterator<char>(serverOutPipe)), std::istreambuf_iterator<char>());
		serverOutPipe.close();

		// Extract JSON from response (skip Content-Length header)
		size_t jsonStart = response.find("\r\n\r\n");
		std::string jsonResponse = "";
		if (jsonStart != std::string::npos) {
			jsonResponse = response.substr(jsonStart + 4);
		}

		// Clean up
		kill(serverPid, SIGTERM);
		waitpid(serverPid, nullptr, 0);
		unlink(tmpIn.c_str());
		unlink(tmpOut.c_str());

		if (jsonResponse.length() > 0) {
			state->status = "LSP Response: " + jsonResponse.substr(0, 50) + "...";
		} else {
			state->status = "No LSP response received";
		}
		copyToClipboard(state, lspMessage, true);
	} else {
		// Fork failed
		state->status = "Failed to start LSP server";
		unlink(tmpIn.c_str());
		unlink(tmpOut.c_str());
	}
}

void tsHelloWorld(State *state, pid_t serverPid)
{
	// This function is kept for compatibility but not used anymore
	tsHelloWorld(state);
}
