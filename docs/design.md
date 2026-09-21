# Initial Design

## Problem

When a coding solution fails on a very large testcase, the testcase
may be too large to manually inspect. The goal is to reduce it to
a smaller testcase that still exposes the correctness failure.

## V1 Scope

- The first version will operate on integer-array test cases.
- Focus on wrong-answer/correctness failures.
- TLE is outside the initial scope.
- The candidate and oracle are provided as separate executables.
- The failing input is provided as input to the tool.
- Input size is derived from the testcase rather than supplied separately.
- A smaller testcase means fewer elements.
- The reduced testcase must preserve the failure condition:
  candidate output != oracle output.

## Initial Questions

- How should a testcase be reduced efficiently?
- Should we remove individual elements or larger chunks?
- How should output comparison work?
- What should happen if the candidate crashes?
- What should happen if the oracle crashes?

## Failure Definition

A testcase is considered a correctness failure when both the
candidate and oracle terminate successfully, but their outputs differ:

candidate output != oracle output