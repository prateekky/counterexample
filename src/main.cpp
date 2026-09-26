#include "comparator.h"
#include "reducer.h"
#include "runner.h"
#include "testcase.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr
            << "Usage: counterexample "
            << "<candidate> <oracle> <input>\n";
        return 1;
    }

    try {
        const string candidate_path = argv[1];
        const string oracle_path = argv[2];
        const string input_path = argv[3];

        const TestCase initial = TestCase::fromFile(input_path);
        const string input = initial.serialize();

        const ExecutionResult candidate =
            runExecutable(candidate_path, input);

        const ExecutionResult oracle =
            runExecutable(oracle_path, input);

        if (candidate.timed_out) {
            cout << "Status: CANDIDATE_TIMEOUT\n";
            return 0;
        }

        if (oracle.timed_out) {
            cout << "Status: ORACLE_TIMEOUT\n";
            return 0;
        }

        if (candidate.exit_code != 0) {
            cout << "Status: CANDIDATE_ERROR\n";
            return 0;
        }

        if (oracle.exit_code != 0) {
            cout << "Status: ORACLE_ERROR\n";
            return 0;
        }

        cout << "Candidate output: "
                  << candidate.stdout_output;

        cout << "Oracle output:    "
                  << oracle.stdout_output;

        if (!isWrongAnswer(candidate, oracle)) {
            cout << "Status: PASS\n";
            return 0;
        }

        const TestCase reduced =
            reduceTestCase(
                initial,
                [&](const TestCase& test_case) {
                    const string reduced_input =
                        test_case.serialize();

                    const ExecutionResult candidate_result =
                        runExecutable(
                            candidate_path,
                            reduced_input
                        );

                    const ExecutionResult oracle_result =
                        runExecutable(
                            oracle_path,
                            reduced_input
                        );

                    return isWrongAnswer(
                        candidate_result,
                        oracle_result
                    );
                }
            );

        cout
            << "Status: WRONG_ANSWER\n";

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

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}