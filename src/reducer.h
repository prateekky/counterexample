#pragma once

#include "testcase.h"

#include <functional>

using namespace std;

using FailureChecker = function<bool(const TestCase&)>;

TestCase reduceTestCase(
    const TestCase& initial,
    const FailureChecker& isFailure
);