# Counterexample Lab

## Problem

I often struggle to debug wrong answers when the failing
test case is very large and difficult to dry-run manually.

## Goal

Build a small tool that can take a failing test case and
reduce it to a smaller input that still demonstrates the
same failure.

## Initial scope

Version 1 will work only with integer-array test cases.

## Questions I haven't decided yet

- What exactly should "smallest" mean?
- How should timeouts be handled?
- Should crashes count as failures?
- How should output comparison work?
- How should the reducer choose between multiple
  possible smaller cases?

## Non-goals

- Supporting every possible competitive programming format
- Building a full online judge
- Supporting interactive problems