#include "runner.h"

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

namespace {

struct TempFile {
    int fd{-1};
    string path;

    ~TempFile() {
        if (fd != -1) {
            close(fd);
        }

        if (!path.empty()) {
            unlink(path.c_str());
        }
    }
};

TempFile createTempFile(const char* prefix) {
    string pattern = string("/tmp/") + prefix + "XXXXXX";

    vector<char> buffer(pattern.begin(), pattern.end());
    buffer.push_back('\0');

    const int fd = mkstemp(buffer.data());

    if (fd == -1) {
        throw runtime_error(
            "Failed to create temporary file: " +
            string(strerror(errno))
        );
    }

    return TempFile{fd, buffer.data()};
}

// Helper to guarantee that the entire buffer is written to the file descriptor,
// handling partial writes and signal interruptions (EINTR).
void writeAll(int fd, const char* data, size_t size) {
    size_t total_written = 0;
    while (total_written < size) {
        ssize_t written = write(fd, data + total_written, size - total_written);
        if (written == -1) {
            if (errno == EINTR) {
                continue; // Interrupted by a signal, try again
            }
            throw runtime_error("Failed to write testcase input: " + string(strerror(errno)));
        }
        total_written += static_cast<size_t>(written);
    }
}

string readFile(const string& path) {
    ifstream file(path, ios::binary);

    if (!file) {
        throw runtime_error("Failed to read temporary output file");
    }

    return {
        istreambuf_iterator<char>(file),
        istreambuf_iterator<char>()
    };
}

}  // namespace

ExecutionResult runExecutable(
    const string& executable,
    const string& input,
    int timeout_ms
) {
    TempFile input_file = createTempFile("bug_shrinker_in_");
    TempFile stdout_file = createTempFile("bug_shrinker_out_");
    TempFile stderr_file = createTempFile("bug_shrinker_err_");

    // Write testcase to stdin file.
    writeAll(input_file.fd, input.data(), input.size());

    // Reset input file offset so the child reads from the beginning.
    if (lseek(input_file.fd, 0, SEEK_SET) == -1) {
        throw runtime_error("Failed to rewind testcase input");
    }

    const pid_t pid = fork();

    if (pid == -1) {
        throw runtime_error("fork() failed");
    }

    if (pid == 0) {
        // Child process.

        dup2(input_file.fd, STDIN_FILENO);
        dup2(stdout_file.fd, STDOUT_FILENO);
        dup2(stderr_file.fd, STDERR_FILENO);

        close(input_file.fd);
        close(stdout_file.fd);
        close(stderr_file.fd);

        char* const argv[] = {
            const_cast<char*>(executable.c_str()),
            nullptr
        };

        execvp(argv[0], argv);

        // Only reached if execvp fails.
        dprintf(
            STDERR_FILENO,
            "Failed to execute '%s': %s\n",
            executable.c_str(),
            strerror(errno)
        );
        _exit(127);
    }

    // Parent process.
    close(input_file.fd);
    input_file.fd = -1;

    close(stdout_file.fd);
    stdout_file.fd = -1;

    close(stderr_file.fd);
    stderr_file.fd = -1;

    int status = 0;
    const int sleep_interval_ms = 5;
    int elapsed_ms = 0;

    while (true) {
        const pid_t result = waitpid(pid, &status, WNOHANG);

        if (result == pid) {
            break;
        }

        if (result == -1) {
            throw runtime_error(
                "waitpid() failed: " +
                string(strerror(errno))
            );
        }

        if (elapsed_ms >= timeout_ms) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);

            return {
                -1,
                readFile(stdout_file.path),
                readFile(stderr_file.path),
                true
            };
        }

        this_thread::sleep_for(
            chrono::milliseconds(sleep_interval_ms)
        );

        elapsed_ms += sleep_interval_ms;
    }

    int exit_code = -1;

    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        exit_code = 128 + WTERMSIG(status);
    }

    return {
        exit_code,
        readFile(stdout_file.path),
        readFile(stderr_file.path),
        false
    };
}