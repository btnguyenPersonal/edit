#include "tsServer.h"
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <nlohmann/json.hpp>

// Forward declarations for LSP functions
bool sendLSPMessage(State *state, const nlohmann::json &message);
nlohmann::json readLSPMessage(State *state, int timeoutMs = 2000);
std::vector<TSError> parseLSPDiagnostics(const nlohmann::json &response);

void startTsServer(State *state)
{
	std::lock_guard<std::mutex> lock(state->tsServerMutex);

	if (state->tsServerRunning.load()) {
		state->tsServerStatus = "ts-server already running";
		return;
	}

	// Check if we're in a TypeScript/JavaScript project
	bool hasPackageJson = std::filesystem::exists("package.json");
	bool hasTsConfig = std::filesystem::exists("tsconfig.json");
	bool hasJsFiles = false;
	bool hasTypeScript = false;

	// Check if TypeScript compiler is available
	FILE *tscCheck = popen("tsc --version 2>/dev/null", "r");
	if (tscCheck != nullptr) {
		char versionBuffer[256];
		if (fgets(versionBuffer, sizeof(versionBuffer), tscCheck)) {
			hasTypeScript = true;
		}
		pclose(tscCheck);
	}

	try {
		for (const auto &entry : std::filesystem::recursive_directory_iterator(".")) {
			if (entry.is_regular_file() && entry.path().extension() == ".js") {
				hasJsFiles = true;
				break;
			}
		}
	} catch (...) {
		// Ignore directory traversal errors
	}

	if (!hasPackageJson && !hasTsConfig && !hasJsFiles && !hasTypeScript) {
		state->tsServerStatus = "ts-server failed: no TypeScript/JavaScript project detected";
		return;
	}

	if (!hasPackageJson && !hasTsConfig && !hasJsFiles) {
		state->tsServerStatus = "ts-server failed: no TypeScript/JavaScript project detected";
		return;
	}

	// Start ts-server in a background thread
	state->tsServerRunning.store(true);
	state->tsServerStatus = "ts-server starting...";

	std::thread serverThread([state]() {
		// Create pipes for bidirectional communication
		int stdin_pipe[2];
		int stdout_pipe[2];

		if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
			state->tsServerStatus = "ts-server failed: could not create pipes";
			state->tsServerRunning.store(false);
			return;
		}

		pid_t pid = fork();

		if (pid == 0) {
			// Child process - start the language server
			close(stdin_pipe[1]);
			close(stdout_pipe[0]);

			dup2(stdin_pipe[0], STDIN_FILENO);
			dup2(stdout_pipe[1], STDOUT_FILENO);
			close(STDERR_FILENO);

			const char *args[] = { "npx", "typescript-language-server", "--stdio", nullptr };
			execvp("npx", const_cast<char *const *>(args));
			_exit(1);

		} else if (pid > 0) {
			// Parent process - keep pipe ends for communication
			close(stdin_pipe[0]);
			close(stdout_pipe[1]);

			state->tsServerInput = fdopen(stdin_pipe[1], "w");
			state->tsServerOutput = fdopen(stdout_pipe[0], "r");

			if (state->tsServerInput == nullptr || state->tsServerOutput == nullptr) {
				state->tsServerStatus = "ts-server failed: could not create file streams";
				state->tsServerRunning.store(false);
				return;
			}

			// Set non-buffered I/O
			setvbuf(state->tsServerInput, nullptr, _IONBF, 0);
			setvbuf(state->tsServerOutput, nullptr, _IONBF, 0);

			// Give it a moment to start
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));

			// Check if process is still running
			int status;
			int result = waitpid(pid, &status, WNOHANG);
			if (result == 0) {
				// Send initialization request automatically
				nlohmann::json initRequest = { { "jsonrpc", "2.0" }, { "id", 1 }, { "method", "initialize" }, { "params", { { "processId", 1 }, { "rootUri", "file://" + std::filesystem::current_path().string() }, { "capabilities", { { "textDocument", { { "diagnostic", { { "dynamicRegistration", false } } } } } } } } } };

				std::string jsonStr = initRequest.dump();
				std::string header = "Content-Length: " + std::to_string(jsonStr.length()) + "\r\n\r\n";

				fputs(header.c_str(), state->tsServerInput);
				fputs(jsonStr.c_str(), state->tsServerInput);
				fflush(state->tsServerInput);

				// Wait for initialize response
				char initBuffer[2048];
				if (fgets(initBuffer, sizeof(initBuffer), state->tsServerOutput)) {
					std::string initResponse(initBuffer);
					state->tsServerStatus = "DEBUG: Init response: " + initResponse;

					if (initResponse.find("result") != std::string::npos) {
						state->tsServerStatus = "ts-server initialized";
					} else {
						state->tsServerStatus = "ts-server init failed - no result";
					}
				} else {
					state->tsServerStatus = "ts-server init failed - no response";
				}

				// Send initialized notification
				nlohmann::json initializedNotif = { { "jsonrpc", "2.0" }, { "method", "initialized" } };

				jsonStr = initializedNotif.dump();
				header = "Content-Length: " + std::to_string(jsonStr.length()) + "\r\n\r\n";

				fputs(header.c_str(), state->tsServerInput);
				fputs(jsonStr.c_str(), state->tsServerInput);
				fflush(state->tsServerInput);
			} else {
				state->tsServerStatus = "ts-server failed: could not start typescript-language-server";
				state->tsServerRunning.store(false);
			}
		}

		state->shouldNotReRender.clear();
	});

	serverThread.detach();
}

void stopTsServer(State *state)
{
	std::lock_guard<std::mutex> lock(state->tsServerMutex);

	if (!state->tsServerRunning.load()) {
		state->tsServerStatus = "ts-server not running";
		return;
	}

	// Close pipes if they exist
	if (state->tsServerInput != nullptr) {
		fclose(state->tsServerInput);
		state->tsServerInput = nullptr;
	}
	if (state->tsServerOutput != nullptr) {
		fclose(state->tsServerOutput);
		state->tsServerOutput = nullptr;
	}

	state->tsServerRunning.store(false);
	state->tsServerStatus = "ts-server stopped";
}

bool sendLSPMessage(State *state, const nlohmann::json &message)
{
	if (!state->tsServerInput || !state->tsServerRunning.load()) {
		return false;
	}

	std::string jsonStr = message.dump();
	std::string header = "Content-Length: " + std::to_string(jsonStr.length()) + "\r\n\r\n";

	fputs(header.c_str(), state->tsServerInput);
	fputs(jsonStr.c_str(), state->tsServerInput);
	fflush(state->tsServerInput);
	return true;
}

std::vector<TSError> getTSErrors(State *state)
{
	std::vector<TSError> errors;

	if (!state->tsServerRunning.load() || !state->tsServerOutput) {
		state->tsServerStatus = "ts-server not initialized";
		return errors;
	}

	// Open the document first (required for diagnostics)
	nlohmann::json openRequest = { { "jsonrpc", "2.0" },
				       { "id", 200 },
				       { "method", "textDocument/didOpen" },
				       { "params",
					 { { "textDocument",
					     {
						     { "uri", "file://" + std::filesystem::absolute(state->file->filename).string() }, { "language", "typescript" }, { "text", "" } // We'll handle content changes later
					     } } } } };

	if (!sendLSPMessage(state, openRequest)) {
		state->tsServerStatus = "Failed to send open request";
		return errors;
	}

	// Skip the open response (we don't need it)
	char buffer[2048];
	if (fgets(buffer, sizeof(buffer), state->tsServerOutput) == nullptr) {
		state->tsServerStatus = "No open response from server";
		return errors;
	}

	// Request diagnostics
	nlohmann::json diagnosticsRequest = { { "jsonrpc", "2.0" }, { "id", 201 }, { "method", "textDocument/diagnostic" }, { "params", { { "textDocument", { { "uri", "file://" + std::filesystem::absolute(state->file->filename).string() } } } } } };

	if (!sendLSPMessage(state, diagnosticsRequest)) {
		state->tsServerStatus = "Failed to send diagnostics request";
		return errors;
	}

	// Read Content-Length header
	if (fgets(buffer, sizeof(buffer), state->tsServerOutput) == nullptr) {
		state->tsServerStatus = "No response from server";
		return errors;
	}

	std::string line(buffer);
	state->tsServerStatus = "DEBUG: Response line: " + line;
	size_t pos = line.find("Content-Length:");
	if (pos == std::string::npos) {
		state->tsServerStatus = "DEBUG: No Content-Length header in response: " + line;
		return errors;
	}

	size_t contentLength = std::stoul(line.substr(pos + 15));

	// Read blank lines
	fgets(buffer, sizeof(buffer), state->tsServerOutput);
	fgets(buffer, sizeof(buffer), state->tsServerOutput);

	// Read JSON content with size limit
	std::string jsonContent;
	for (size_t i = 0; i < contentLength && i < 1023; i++) {
		int c = fgetc(state->tsServerOutput);
		if (c == EOF)
			break;
		jsonContent += static_cast<char>(c);
	}

	// Debug: Show what we got
	state->tsServerStatus = "DEBUG: Response JSON: " + jsonContent.substr(0, 200);

	try {
		auto response = nlohmann::json::parse(jsonContent);
		errors = parseLSPDiagnostics(response);

		if (errors.empty()) {
			state->tsServerStatus = "No TypeScript errors found";
		} else {
			state->tsServerStatus = "Found " + std::to_string(errors.size()) + " TypeScript errors";
		}
	} catch (...) {
		state->tsServerStatus = "Failed to parse diagnostics response";
	}

	return errors;
}

std::vector<TSError> parseLSPDiagnostics(const nlohmann::json &response)
{
	std::vector<TSError> errors;

	if (!response.contains("result") || !response["result"].contains("diagnostics")) {
		return errors;
	}

	for (const auto &diag : response["result"]["diagnostics"]) {
		if (!diag.contains("range") || !diag.contains("message")) {
			continue;
		}

		TSError error;
		error.severity = "error";
		error.message = diag["message"];

		if (diag["range"].contains("start")) {
			error.line = diag["range"]["start"]["line"];
			error.column = diag["range"]["start"]["character"];
		}

		errors.push_back(error);
	}

	return errors;
}
