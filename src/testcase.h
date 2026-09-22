#pragma once

#include <string>
#include <vector>

using namespace std;

struct TestCase {
    vector<int> values;

    static TestCase fromFile(const string& path);
    string serialize() const;
};