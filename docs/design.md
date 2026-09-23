# Initial Design

## Problem

When a coding solution fails on a very large testcase, the testcase
may be too large to manually inspect.

The goal of this project is to reduce such a failing testcase to a
smaller testcase that still exposes the correctness failure, making
the failure easier for a human to understand and debug.

The project is motivated by a personal problem: when a solution fails
on a large testcase, manually tracing the complete input is often
impractical. A smaller counterexample can make the underlying bug much
easier to identify.

---

## V1 Scope

The first version deliberately keeps the problem narrow.

- The first version operates on integer-array test cases.
- The first version focuses on wrong-answer/correctness failures.
- TLE is outside the initial scope.
- The candidate and oracle are provided as separate executables.
- The failing input is provided as input to the tool.
- Input size is derived from the testcase rather than supplied separately.
- A smaller testcase means fewer elements.
- Elements may be removed, but their relative order is preserved.
- The reduced testcase must preserve the failure condition.

### Non-goals for V1

- TLE reduction.
- Compilation-error handling.
- Interactive problems.
- Supporting arbitrary testcase formats.
- Supporting every competitive-programming input/output format.
- Building a full online judge.

---

## Design Principle

The project is intentionally being developed experimentally.

Rather than choosing a final algorithm in advance, reduction strategies
are tested on concrete failures and the design is revised based on
observed behavior.

The repository should therefore preserve:

- decisions,
- rejected approaches,
- assumptions,
- experiments,
- failures,
- and changes made in response to those findings.

---


## Initial Reduction Experiments

Before implementing the reducer, two strategies were investigated
manually using a real incorrect solution for the maximum-profit problem.

### One-element removal

The first strategy removes one element at a time and keeps a deletion
if the failure is preserved.

The original 11-element testcase was reduced to:

    [4,9,0,9]

At this point, removing any single element caused the candidate and
oracle to agree.

This demonstrated that one-element deletion can produce a useful
counterexample, but it may require many candidate/oracle executions.

### Chunk removal

The second strategy removes larger contiguous chunks.

The manual experiment initially used a 6+5 split for the 11-element
testcase. Removing the first 6-element chunk preserved the failure and
reduced the testcase from 11 elements to 5 elements.

At the 5-element testcase, neither half could be removed while
preserving the failure. Increasing the granularity to four chunks then
allowed a further reduction to:

    [4,9,0,9]

This experiment suggested that larger chunks can remove irrelevant
input more efficiently, while finer granularity is necessary when a
failure depends on elements distributed across multiple chunks.

For the implementation, the partitioning rule was later defined so
that remainder elements are assigned to the last chunks. Therefore,
the implementation will partition 11 elements into two chunks as:

    [5] [6]

---

## What We Learned
- One-element deletion can reduce the testcase, but may require many
  candidate/oracle executions.
- Chunk deletion can remove several elements in fewer executions.
- Removing a large chunk does not always preserve the failure.
- Increasing the number of chunks provides finer-grained reduction when
  a coarser partition cannot make progress.

---

## Failure Definition

A testcase is considered a correctness failure when both the candidate
and oracle terminate successfully, but their outputs differ:

    candidate output != oracle output

The tool therefore treats the oracle as the reference for determining
whether a correctness failure is preserved.

---

## Inputs to the Tool

The conceptual inputs are:

1. A candidate executable.
2. A trusted oracle/reference executable.
3. A failing testcase.

The tool should verify that the provided testcase actually produces
different outputs before attempting to reduce it.

The expected and incorrect output values do not need to be supplied
separately because the tool can execute both programs and determine
the outputs itself.

---

## Reduction Goal

The primary reduction objective for V1 is:

> Find a testcase with fewer elements that still causes the candidate
> and oracle to produce different outputs.

For example:

    Original:
    [1,2,4,2,5,7,2,4,9,0,9]

    Candidate -> 8
    Oracle    -> 9

    Reduced:
    [4,9,0,9]

    Candidate -> 5
    Oracle    -> 9

The output values themselves do not need to remain the same.
Only the failure condition must be preserved.

---

## Implementation Language

The tool will initially be implemented in C++20.

The experiments and candidate/oracle programs are already written in
C++, so using C++20 keeps the experimental environment consistent while
allowing the tool itself to remain independent of the specific candidate
algorithm.

## Execution Model

The candidate and oracle will be executed as separate processes.

The testcase will be serialized and provided through standard input.

The execution layer will capture standard output and standard error and
will enforce a timeout so that a hanging program cannot hang the
shrinker itself.

A timeout is not considered a correctness failure in V1.

## Output Comparison

For V1, program outputs will be compared as whitespace-separated tokens.

Differences in leading, trailing, or repeated whitespace will not
constitute a correctness difference. A difference in token values or
token count will constitute a difference.

Only stdout is used for correctness comparison.
stderr is ignored.

---

## Reducer Design

### Initial Granularity

Based on the experiments, V1 will start with chunk-based reduction.

The initial number of chunks is:

    k = 2

If no chunk can be removed while preserving the failure, the number of
chunks is increased by powers of two:

    2 -> 4 -> 8 -> 16 -> ...

At every step, the number of chunks is at most the number of
elements, so every chunk is non-empty.

---

### Chunk Partitioning

Chunks are contiguous and preserve the relative order of elements.

The elements are divided as evenly as possible.

If the number of elements is not evenly divisible by the number of
chunks, the extra elements are assigned to the last chunks.

For example:

    11 elements, 4 chunks

becomes:

    [2] [3] [3] [3]

Similarly:

    10 elements, 4 chunks

    [2] [2] [3] [3]

and:

    9 elements, 4 chunks
    [2] [2] [2] [3]

In general:

    base_size = n / k
    remainder = n % k

The first (k - remainder) chunks contain `base_size` elements.
The last `remainder` chunks contain `base_size + 1` elements.

---

### Chunk Traversal Order

Chunks are tested from left to right.

When multiple chunks can be removed, the first successful removal is accepted.

Alternative traversal orders, such as right-to-left or
testing both directions, are not part of V1.

---

### Behavior After a Successful Reduction

For each chunk:

1. Temporarily remove the chunk.
2. Run the candidate and oracle on the remaining testcase.
3. Compare their outputs.
4. If they still differ, accept the reduction.
5. Discard the previously computed chunks because they were based on
   the old testcase.
6. Repartition the new testcase using the same number of chunks `k`.
7. Start a new pass from the first chunk.

The value of `k` is therefore unchanged after a successful reduction.

---

### Granularity Growth

If no chunk can be removed at the current granularity, increase the
number of chunks:

    k = min(2 * k, current number of elements)

If the chunk count reaches the number of elements, each chunk contains
one element.

If no single-element deletion preserves the failure, reduction stops.

---

### Reduction Algorithm

The current conceptual algorithm is:

    k = 2

    while reduction is possible:

        k = min(k, current testcase size)

        split testcase into k contiguous chunks

        reduction_found=false

        for each chunk from left to right:

            remove the chunk

            run candidate and oracle

            if outputs differ:
                accept the reduced testcase
                reduction_found=true
                break

        if reduction_found:
            continue

At termination, no individual element can be removed while preserving
the failure when the chunk count has reached the number of elements.

---

## Current Assumptions

- The oracle is trusted to produce the correct result.
- The candidate and oracle accept the same testcase format.
- The testcase can be represented as an integer array.
- Removing elements from an array produces another valid testcase
  after updating its size.
- The failure condition can be determined by comparing program outputs.
- Relative element order is significant and therefore must be preserved.

---

## What "Smallest" Means

For V1, "smaller" means:

> fewer elements.

The current objective is therefore to minimize the number of elements,
not necessarily the magnitude of the values or the textual length of
the testcase.

A globally smallest counterexample is not guaranteed by the current
strategy. The reducer is instead designed to efficiently find a small
failure-preserving testcase.

At the point where each chunk contains a single element and no single
element can be removed, the resulting testcase is 1-minimal with
respect to element deletion.

---

## Open Questions

The following decisions have not yet been finalized:

- What should happen if the candidate crashes?
- What should happen if the oracle crashes?
- How should malformed candidate/oracle execution be reported?
- What should happen when the initial testcase does not actually fail?

These questions are intentionally left open until implementation and
experimentation provide enough evidence to make the decisions.