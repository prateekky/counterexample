#include "testcase.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: testcase_test <input-file>\n";
        return 1;
    }

    try {
        const TestCase test_case = TestCase::fromFile(argv[1]);

        cout << test_case.serialize();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}