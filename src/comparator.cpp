#include "comparator.h"

#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

vector<string> tokenizeOutput(const string& output) {
    istringstream stream(output);

    vector<string> tokens;
    string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

bool outputsMatch(
    const string& candidate_output,
    const string& oracle_output
) {
    return tokenizeOutput(candidate_output) ==
           tokenizeOutput(oracle_output);
}

}  // namespace

bool isWrongAnswer(
    const ExecutionResult& candidate,
    const ExecutionResult& oracle
) {
    // V1 only considers cases where both programs
    // terminate successfully.
    if (candidate.timed_out || oracle.timed_out) {
        return false;
    }

    if (candidate.exit_code != 0 || oracle.exit_code != 0) {
        return false;
    }

    return !outputsMatch(
        candidate.stdout_output,
        oracle.stdout_output
    );
}