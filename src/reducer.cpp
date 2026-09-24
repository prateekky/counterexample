#include "reducer.h"

#include <algorithm>
#include <cstddef>
#include <vector>

using namespace std;

namespace {

struct Chunk {
    size_t begin;
    size_t end;  // exclusive
};

vector<Chunk> splitIntoChunks(
    size_t size,
    size_t chunk_count
) {
    chunk_count = min(chunk_count, size);

    vector<Chunk> chunks;
    chunks.reserve(chunk_count);

    const size_t base_size = size / chunk_count;
    const size_t remainder = size % chunk_count;

    size_t begin = 0;

    for (size_t i = 0; i < chunk_count; ++i) {
        const size_t chunk_size =
            base_size + (i >= chunk_count - remainder ? 1 : 0);

        chunks.push_back({begin, begin + chunk_size});
        begin += chunk_size;
    }

    return chunks;
}

TestCase removeChunk(
    const TestCase& test_case,
    const Chunk& chunk
) {
    TestCase reduced;

    reduced.values.reserve(
        test_case.values.size() - (chunk.end - chunk.begin)
    );

    reduced.values.insert(
        reduced.values.end(),
        test_case.values.begin(),
        test_case.values.begin() + chunk.begin
    );

    reduced.values.insert(
        reduced.values.end(),
        test_case.values.begin() + chunk.end,
        test_case.values.end()
    );

    return reduced;
}

} //namespace


TestCase reduceTestCase(
    const TestCase& initial,
    const FailureChecker& isFailure
) {
    TestCase current = initial;
    size_t chunk_count = 2;

    while (current.values.size() > 1) {
        const size_t size = current.values.size();

        chunk_count = min(chunk_count, size);

        const auto chunks =
            splitIntoChunks(size, chunk_count);

        bool reduction_found = false;

        for (const Chunk& chunk : chunks) {
            TestCase reduced = removeChunk(current, chunk);

            if (isFailure(reduced)) {
                current = move(reduced);
                reduction_found = true;
                break;
            }
        }

        if (reduction_found) {
            continue;
        }

        if (chunk_count == size) {
            break;
        }

        chunk_count = min(chunk_count * 2, size);
    }

    return current;
}