cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 23)
project(OnyxCPPTesting)

include(FetchContent)

# Fetch GoogleTest from GitHub
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Prevent GTest from using dynamic CRT on Windows
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(OnyxCPPTests CoreTests.cpp)
target_link_libraries(OnyxCPPTests PRIVATE OnyxCoreLib gtest_main)

include(GoogleTest)
gtest_discover_tests(OnyxCPPTests)