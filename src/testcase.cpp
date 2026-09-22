#include "testcase.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

TestCase TestCase::fromFile(const string& path) {
    ifstream file(path);

    if (!file) {
        throw runtime_error("Failed to open testcase: " + path);
    }

    int n;
    if (!(file >> n)) {
        throw runtime_error("Failed to read testcase size");
    }

    if (n < 0) {
        throw runtime_error("Testcase size cannot be negative");
    }

    TestCase test_case;
    test_case.values.reserve(n);

    for (int i = 0; i < n; ++i) {
        int value;

        if (!(file >> value)) {
            throw runtime_error(
                "Testcase ended before reading all elements"
            );
        }

        test_case.values.push_back(value);
    }

    string extra;

    if(file >> extra){
        throw runtime_error(
            "Testcase contains extra input after the declared number of elements"
        );
    }

    return test_case;
}

string TestCase::serialize() const {
    ostringstream output;

    output << values.size() << '\n';

    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            output << ' ';
        }

        output << values[i];
    }

    output << '\n';

    return output.str();
}