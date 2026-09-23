#include "comparator.h"

#include <cassert>
#include <iostream>
#include <string>

ExecutionResult success(const std::string& output) {
    return {
        0,      // exit_code
        output,
        "",     // stderr
        false   // timed_out
    };
}

ExecutionResult crash() {
    return {
        134,
        "",
        "",
        false
    };
}

ExecutionResult timeout() {
    return {
        -1,
        "",
        "",
        true
    };
}

int main() {
    // Same output.
    assert(!isWrongAnswer(
        success("9\n"),
        success("9\n")
    ));

    // Different output.
    assert(isWrongAnswer(
        success("8\n"),
        success("9\n")
    ));

    // Whitespace differences should be ignored.
    assert(!isWrongAnswer(
        success("  9\n"),
        success("9")
    ));

    // Multiple whitespace-separated tokens.
    assert(!isWrongAnswer(
        success("9  10\n"),
        success("9 10")
    ));

    // Different token count/value.
    assert(isWrongAnswer(
        success("9 10"),
        success("9")
    ));

    // stderr should not matter.
    assert(!isWrongAnswer(
        {0, "9\n", "debug message\n", false},
        {0, "9\n", "", false}
    ));

    // Candidate crashes.
    assert(!isWrongAnswer(
        crash(),
        success("9")
    ));

    // Oracle crashes.
    assert(!isWrongAnswer(
        success("9"),
        crash()
    ));

    // Candidate times out.
    assert(!isWrongAnswer(
        timeout(),
        success("9")
    ));

    // Oracle times out.
    assert(!isWrongAnswer(
        success("9"),
        timeout()
    ));

    std::cout << "All comparator tests passed.\n";
}