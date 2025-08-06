################################################
# Testing setup.
# Installs and uses google test to run Unit testss.
################################################

cmake_minimum_required(VERSION 3.16.3)
# Defines the build target for tests and also contains source code for more advanced tests.
project(Unit-Tests)

# Allow ctest to find the tests we've written
enable_testing()

# FetchContent will help with automatic dependency resolution.

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Detecting and including all tests
file(GLOB_RECURSE TEST_SRCS ../lib/validation/*.cpp)

# Same scope of directories as the main project
include_directories(../lib/validation/)

# Including Mocks and Fakes for the projects
include_directories(.)

# Definition can be used in the code to enable testing of hardware coupled functions.
add_definitions("-DUNIT_TESTING=1")

add_executable(unit_tests ${TEST_SRCS})
target_link_libraries(unit_tests GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(unit_tests)