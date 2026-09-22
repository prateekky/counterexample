# Reduction Experiment 1

Original:
[1,2,4,2,5,7,2,4,9,0,9]

Candidate output: 8
Oracle output: 9

## One-element removal from 11 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 10 | 7 | 9 | Yes |
| 1 | 10 | 8 | 9 | Yes |
| 2 | 10 | 8 | 9 | Yes |
| 3 | 10 | 8 | 9 | Yes |
| 4 | 10 | 8 | 9 | Yes |
| 5 | 10 | 8 | 9 | Yes |
| 6 | 10 | 8 | 9 | Yes |
| 7 | 10 | 8 | 9 | Yes |
| 8 | 10 | 7 | 9 | Yes |
| 9 | 10 | 8 | 8 | No |
| 10 | 10 | 8 | 8 | No |

### Observation

Removing most individual elements from the original testcase preserves
the correctness failure. Removing either of the final two elements
destroys it.

I will continue with greedy one-element deletion to see how small a
counterexample this strategy can produce.

## One-element removal from 10 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 9 | 7 | 9 | Yes |
| 1 | 9 | 7 | 9 | Yes |
| 2 | 9 | 5 | 9 | Yes |
| 3 | 9 | 7 | 9 | Yes |
| 4 | 9 | 7 | 9 | Yes |
| 5 | 9 | 7 | 9 | Yes |
| 6 | 9 | 7 | 9 | Yes |
| 7 | 9 | 4 | 9 | Yes |
| 8 | 9 | 7 | 7 | No |
| 9 | 9 | 7 | 7 | No |

## One-element removal from 9 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 8 | 7 | 9 | Yes |
| 1 | 8 | 7 | 9 | Yes |
| 2 | 8 | 7 | 9 | Yes |
| 3 | 8 | 7 | 9 | Yes |
| 4 | 8 | 7 | 9 | Yes |
| 5 | 8 | 5 | 9 | Yes |
| 6 | 8 | 7 | 9 | Yes |
| 7 | 8 | 5 | 9 | Yes |
| 8 | 8 | 5 | 9 | Yes |

## One-element removal from 8 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 7 | 7 | 9 | Yes |
| 1 | 7 | 7 | 9 | Yes |
| 2 | 7 | 7 | 9 | Yes |
| 3 | 7 | 7 | 9 | Yes |
| 4 | 7 | 5 | 9 | Yes |
| 5 | 7 | 7 | 9 | Yes |
| 6 | 7 | 5 | 9 | Yes |
| 7 | 7 | 5 | 9 | Yes |

### Observation

Every single-element deletion tested so far has preserved the failure.
The greedy strategy therefore continues to remove the first element
that preserves the failure.

The testcase has been reduced from 11 elements to 7 elements.

## One-element removal from 7 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 6 | 7 | 9 | Yes |
| 1 | 6 | 7 | 9 | Yes |
| 2 | 6 | 5 | 9 | Yes |
| 3 | 6 | 7 | 9 | Yes |
| 4 | 6 | 5 | 9 | Yes |
| 5 | 6 | 7 | 7 | No |
| 6 | 6 | 7 | 7 | No |

## One-element removal from 6 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 5 | 7 | 9 | Yes |
| 1 | 5 | 5 | 9 | Yes |
| 2 | 5 | 7 | 9 | Yes |
| 3 | 5 | 5 | 9 | Yes |
| 4 | 5 | 7 | 7 | No |
| 5 | 5 | 7 | 7 | No |

## One-element removal from 5 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 4 | 5 | 9 | Yes |
| 1 | 4 | 7 | 9 | Yes |
| 2 | 4 | 5 | 9 | Yes |
| 3 | 4 | 7 | 7 | No |
| 4 | 4 | 7 | 7 | No |

## One-element removal from 4 elements

| Removed index | Remaining size | Candidate | Oracle | Still fails? |
|---:|---:|---:|---:|---|
| 0 | 3 | 9 | 9 | No |
| 1 | 3 | 9 | 9 | No |
| 2 | 3 | 5 | 5 | No |
| 3 | 3 | 5 | 5 | No |

## Result of One-Element Reduction

Starting testcase:

[1,2,4,2,5,7,2,4,9,0,9]

Using greedy one-element deletion, the testcase was reduced to:

[4,9,0,9]

Candidate output: 5
Oracle output: 9

Every single-element deletion from [4,9,0,9] removes the
correctness failure. Therefore, this testcase is 1-minimal
under the deletion operation.

### Observation

For this particular candidate bug, the reduction reached a
4-element counterexample. A 3-element testcase is insufficient
to expose this bug, so 4 is the minimum testcase length for
this experiment.

This does not establish that greedy one-element deletion
always produces a globally minimal testcase.