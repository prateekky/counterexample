#include "comparator.h"
#include "reducer.h"
#include "runner.h"
#include "testcase.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;

bool isFailure(
    const string& candidate_path,
    const string& oracle_path,
    const TestCase& test_case
) {
    const string input = test_case.serialize();

    const ExecutionResult candidate =
        runExecutable(candidate_path, input);

    const ExecutionResult oracle =
        runExecutable(oracle_path, input);

    return isWrongAnswer(candidate, oracle);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr
            << "Usage: integration_test "
            << "<candidate> <oracle> <input>\n";
        return 1;
    }

    const string candidate_path = argv[1];
    const string oracle_path = argv[2];
    const string input_path = argv[3];

    const TestCase initial =
        TestCase::fromFile(input_path);

    // First verify that the starting testcase really fails.
    assert(isFailure(
        candidate_path,
        oracle_path,
        initial
    ));

    const TestCase reduced =
        reduceTestCase(
            initial,
            [&](const TestCase& test_case) {
                return isFailure(
                    candidate_path,
                    oracle_path,
                    test_case
                );
            }
        );

    // The reducer must preserve the failure.
    assert(isFailure(
        candidate_path,
        oracle_path,
        reduced
    ));

    // The reducer should actually reduce the testcase.
    assert(
        reduced.values.size() <
        initial.values.size()
    );

    cout
        << "Initial size: "
        << initial.values.size()
        << '\n';

    cout
        << "Reduced size: "
        << reduced.values.size()
        << '\n';

    cout << "Reduced testcase:\n";
    cout << reduced.serialize();

    cout << "Integration test passed.\n";
}