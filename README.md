# Counterexample Lab

A small C++ tool for shrinking large failing test cases into smaller, human-understandable counterexamples.

## Problem

When a competitive programming solution produces a wrong answer on a large test case, manually debugging the input can be difficult.

The useful question is:

> What is a much smaller input I can find that still exposes the same
> correctness failure?

Counterexample Lab automates this reduction.

## How It Works

The tool takes:

- a candidate executable
- a trusted oracle executable
- a failing integer-array testcase

It repeatedly removes contiguous chunks of the testcase and keeps a
reduction only when the candidate and oracle still disagree.

The current reducer:

1. Starts with 2 chunks.
2. Tries removing chunks from left to right.
3. Accepts the first removal that preserves the failure.
4. Repartitions the reduced testcase and repeats.
5. If no reduction is possible, doubles the number of chunks.
6. Eventually reaches single-element granularity.

The relative order of the remaining elements is preserved.

## Usage

Build the executable with CMake:

```bash
cmake -S . -B build
cmake --build build
```

The executable is created at:

```text
build/counterexample
```

Run:

```bash
./build/counterexample <candidate> <oracle> <input>
```

Example:

```bash
./build/counterexample \
    experiments/max_profit/candidate \
    experiments/max_profit/oracle \
    experiments/max_profit/cases/failing.txt
```

Example output:

```text
Candidate output: 8
Oracle output:    9
Status: WRONG_ANSWER
Initial size: 11
Reduced size: 4
Reduced testcase:
4
4 9 0 9
```

## Current Scope

Version 1 supports:

- integer-array testcases
- correctness / wrong-answer failures
- separate candidate and oracle executables
- whitespace-insensitive stdout comparison
- execution timeouts as a safety mechanism
- deterministic greedy reduction

A testcase is considered failing when both executions terminate successfully and their stdout outputs differ.

## Reduction Strategy

The reducer uses a progressively finer-grained search.

It starts with two contiguous chunks. If removing a chunk preserves the failure, that reduction is accepted immediately and the testcase is repartitioned using the same chunk count.

If no chunk can be removed, the number of chunks is doubled:

```text
2 → 4 → 8 → 16 → ...
```

The number of chunks is capped at the current testcase size.

Chunks are traversed from left to right, and the first successful reduction is accepted.

When the reducer reaches single-element granularity and no individual element can be removed while preserving the failure, reduction stops.

## Important Limitation

The reducer does **not** guarantee a globally smallest failing testcase.

Its reduction strategy is greedy. It can reach a testcase where no single allowed reduction preserves the failure even though a smaller failing testcase exists elsewhere in the search space.

The repository contains a synthetic test demonstrating this behavior.

Therefore, the result should be understood as a reduced testcase that is 1-minimal with respect to single-element deletion when the reducer reaches single-element granularity, not necessarily the globally smallest counterexample.

## Failure Definition

For Version 1, a testcase is considered a correctness failure when:

1. the candidate terminates successfully,
2. the oracle terminates successfully, and
3. their stdout outputs differ when compared as whitespace-separated tokens.

Differences in whitespace alone do not count as failures.

Standard error is ignored for output comparison.

Timeouts and non-zero exit codes are treated as execution errors rather than wrong-answer failures.

## Experiments

The repository contains experiments based on a buggy `maxProfit`
implementation.

The original failing testcase contains 11 elements:

```text
11
1 2 4 2 5 7 2 4 9 0 9
```

The candidate produces:

```text
8
```

while the trusted oracle produces:

```text
9
```

The reducer finds:

```text
4
4 9 0 9
```

The reduced testcase still produces different outputs between the candidate and oracle.

### Large Input Experiment

The same bug was also tested on a generated testcase containing 100,000 elements.

The tool reduced:

```text
100000 elements
```

to:

```text
4 elements
```

with the same counterexample:

```text
4
4 9 0 9
```

For the large experiment, a faster trusted oracle is used because the brute-force reference implementation is intentionally too expensive for large inputs.

The brute-force oracle is useful for small testcases because its behavior is easy to understand and serves as a straightforward reference implementation.

## Synthetic Greedy Experiment

A controlled synthetic failure predicate was used to demonstrate the limitation of greedy reduction.

The initial testcase is:

```text
0 1 2 3
```

The failure predicate considers these inputs failing:

```text
0 1 2 3
0 2 3
1 3
```

The reducer produces:

```text
0 2 3
```

even though:

```text
1 3
```

is a smaller failing testcase.

This demonstrates that the current greedy strategy does not guarantee a globally smallest counterexample.

The experiment is intentionally synthetic and is used to validate a property of the reduction algorithm rather than to model a specific real-world program bug.

## Repository Structure

```text
src/
├── comparator.cpp
├── comparator.h
├── main.cpp
├── reducer.cpp
├── reducer.h
├── runner.cpp
├── runner.h
├── testcase.cpp
└── testcase.h

tests/
├── comparator_test.cpp
├── integration_test.cpp
└── reducer_test.cpp

docs/
└── design.md

experiments/
└── max_profit/
    ├── candidate.cpp
    ├── oracle.cpp
    ├── oracle_fast.cpp
    ├── cases/
    │   ├── failing.txt
    │   ├── generate_large_case.py
    │   ├── large.txt
    │   └── test.txt
    └── notes.md

## Running Tests

### Comparator tests

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic \
    tests/comparator_test.cpp \
    src/comparator.cpp \
    -Isrc \
    -o comparator_test

./comparator_test
```

### Reducer tests

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic \
    tests/reducer_test.cpp \
    src/reducer.cpp \
    src/testcase.cpp \
    -Isrc \
    -o reducer_test

./reducer_test
```

### Integration test

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic \
    tests/integration_test.cpp \
    src/testcase.cpp \
    src/runner.cpp \
    src/comparator.cpp \
    src/reducer.cpp \
    -Isrc \
    -o integration_test
```

Run it with a candidate, oracle, and failing testcase:

```bash
./integration_test \
    <candidate> \
    <oracle> \
    <input>
```

## Implementation

The project is written in C++20.

### Testcase Parser

`testcase.cpp` parses the integer-array testcase format:

```text
n
a1 a2 a3 ... an
```

The parser validates that:

- the declared size is non-negative,
- all `n` elements are present,
- no extra input remains after the declared elements.

### Process Runner

`runner.cpp` executes candidate and oracle programs in separate processes.

It captures:

- standard output
- standard error
- exit code
- timeout status

The runner uses a timeout as a safety mechanism so a hanging program does not block reduction indefinitely.

### Comparator

`comparator.cpp` compares candidate and oracle stdout as
whitespace-separated tokens.

Therefore, these outputs are considered equivalent:

```text
9
```

and:

```text
9
```

even when their whitespace formatting differs.

### Reducer

`reducer.cpp` implements the greedy chunk-based reduction strategy described above.

The reducer does not know why a testcase fails. It only receives a failure checker that answers:

```text
Does this testcase still reproduce the failure?
```

This keeps the reduction algorithm independent from any particular problem or candidate program.

## Non-Goals

Version 1 does not attempt to:

- support arbitrary competitive-programming input formats
- act as a complete online judge
- support interactive problems
- guarantee the globally smallest counterexample
- perform time-limit-exceeded reduction

## Future Work

Possible extensions include:

- supporting additional testcase formats
- better reporting of candidate/oracle execution errors
- more sophisticated reduction strategies
- configurable timeout values
- reduction of other classes of failures
- comparing alternative search strategies such as different chunk traversal
  orders or backtracking

These are intentionally outside the current Version 1 scope.