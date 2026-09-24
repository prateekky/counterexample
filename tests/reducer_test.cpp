#include "reducer.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

bool containsNine(const TestCase& test_case) {
    return find(
        test_case.values.begin(),
        test_case.values.end(),
        9
    ) != test_case.values.end();
}

bool alwaysFails(const TestCase&) {
    return true;
}

int main() {
    {
        TestCase initial{{1, 2, 3, 4, 9, 5}};

        const TestCase reduced =
            reduceTestCase(initial, containsNine);

        assert(containsNine(reduced));
        assert(reduced.values.size() < initial.values.size());
    }

    {
        TestCase initial{{9}};

        const TestCase reduced =
            reduceTestCase(initial, containsNine);

        assert(reduced.values == initial.values);
    }

    {
        TestCase initial{{1, 2}};

        const TestCase reduced =
            reduceTestCase(initial, alwaysFails);

        assert(!reduced.values.empty());
        assert(reduced.values.size() == 1);
    }

    {
        TestCase initial{{1}};

        const TestCase reduced =
            reduceTestCase(initial, alwaysFails);

        assert(reduced.values == initial.values);
    }

    std::cout << "All reducer tests passed.\n";
}