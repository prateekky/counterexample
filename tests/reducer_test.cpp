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

bool containsKnownBadSubset(const TestCase& test_case) {
    const auto& v = test_case.values;

    bool has0 = find(v.begin(), v.end(), 0) != v.end();
    bool has1 = find(v.begin(), v.end(), 1) != v.end();
    bool has2 = find(v.begin(), v.end(), 2) != v.end();
    bool has3 = find(v.begin(), v.end(), 3) != v.end();
    bool has4 = find(v.begin(), v.end(), 4) != v.end();
    bool has5 = find(v.begin(), v.end(), 5) != v.end();

    // Known minimal failing testcase: {0, 2, 5}
    // Additional 4-element failing combinations make the
    // greedy reduction path potentially choose a larger result.
    return (has0 && has2 && has5) ||
           (has1 && has2 && has3 && has5) ||
           (has1 && has2 && has4 && has5) ||
           (has0 && has2 && has3 && has4);
}

bool greedyTrap(const TestCase& tc) {
    const auto& v = tc.values;

    // Three deliberately chosen failing testcases:
    // initial: [0,1,2,3]
    // greedy path: [0,2,3]
    // globally smaller failing case: [1,3]
    return v == vector<int>{0,1,2,3} ||
           v == vector<int>{0,2,3} ||
           v == vector<int>{1,3};
}

int main() {
    {
        TestCase initial{{0, 1, 2, 3}};

        assert(greedyTrap(initial));

        const TestCase reduced =
            reduceTestCase(initial, greedyTrap);

        assert(greedyTrap(reduced));
        
        //smaller failing testcase
        const TestCase known_smaller{{1,3}};
        assert(greedyTrap(known_smaller));

        // The greedy reducer does not necessarily find the global minimum.
        assert(reduced.values.size() == 3);

        cout << "Greedy trap testcase: \n"
        << reduced.serialize();
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


    cout << "All reducer tests passed.\n";
}