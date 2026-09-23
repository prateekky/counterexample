#pragma once

#include <string>

using namespace std;

struct ExecutionResult {
    int exit_code;
    string stdout_output;
    string stderr_output;
    bool timed_out;
};

ExecutionResult runExecutable(
    const string& executable,
    const string& input,
    int timeout_ms = 2000
);