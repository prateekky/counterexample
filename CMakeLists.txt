cmake_minimum_required(VERSION 3.20)

project(counterexample
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(counterexample
    src/main.cpp
    src/testcase.cpp
    src/runner.cpp
    src/comparator.cpp
    src/reducer.cpp
)

target_include_directories(counterexample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(counterexample PRIVATE
    cxx_std_20
)

target_compile_options(counterexample PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
