#include "tsServer.h"
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

	if (!hasPackageJson && !hasTsConfig && !hasJsFiles) {
		state->tsServerStatus = "ts-server failed: no TypeScript/JavaScript project detected";
		return;
	}

	// Start ts-server in a background thread
	state->tsServerRunning.store(true);
	state->tsServerStatus = "ts-server starting...";

	std::thread serverThread([state]() {
		// Try to start typescript language server using fork/exec
		std::vector<std::string> commands = { "npx", "typescript-language-server", "--stdio" };

		bool started = false;
		pid_t pid = fork();

		if (pid == 0) {
			// Child process - start the language server
			std::vector<char *> args;
			for (const auto &cmd : commands) {
				args.push_back(const_cast<char *>(cmd.c_str()));
			}
			args.push_back(nullptr);

			// Redirect stdin/stdout/stderr to /dev/null to detach
			freopen("/dev/null", "r", stdin);
			freopen("/dev/null", "w", stdout);
			freopen("/dev/null", "w", stderr);

			execvp(args[0], args.data());
			// If we get here, exec failed
			_exit(1);
		} else if (pid > 0) {
			// Parent process - check if child started successfully
			int status;
			waitpid(pid, &status, WNOHANG); // Non-blocking wait

			// Give it a moment to start
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			// Check if process is still running
			int result = waitpid(pid, &status, WNOHANG);
			if (result == 0) {
				// Process is still running
				started = true;
			}
		}

		if (started) {
			{
				std::lock_guard<std::mutex> statusLock(state->tsServerMutex);
				state->tsServerStatus = "ts-server initialized";
			}
		} else {
			state->tsServerRunning.store(false);
			{
				std::lock_guard<std::mutex> statusLock(state->tsServerMutex);
				state->tsServerStatus = "ts-server failed: could not start typescript-language-server";
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

	state->tsServerRunning.store(false);
	state->tsServerStatus = "ts-server stopped";
}
