cmake_minimum_required(VERSION 3.10)
project(expresso C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for source code
add_subdirectory(src/core)
add_subdirectory(src/parser)
add_subdirectory(src/cli)
# Placeholder for the main executable
add_executable(expresso src/cli/main.c src/cli/cli_interface.cpp)
target_link_libraries(expresso expresso_core expresso_parser)
target_include_directories(expresso PUBLIC src/parser src/cli) # Add src/cli to includes

# Add the C integration test executable
add_executable(test_interactive_session tests/integration/test_interactive_session.c)
# This test will eventually link against the CLI executable
# target_link_libraries(test_interactive_session expresso_cli)

# Add C unit test executables
add_executable(test_value tests/unit/core/test_value.c)
target_link_libraries(test_value expresso_core)

add_executable(test_evaluator tests/unit/core/test_evaluator.c)
target_link_libraries(test_evaluator expresso_core)

add_executable(test_history tests/unit/core/test_history.c)
target_link_libraries(test_history expresso_core)

# Add subdirectories for source code
add_subdirectory(src/core)
add_subdirectory(src/parser)
add_subdirectory(src/cli)

# Add GoogleTest as a subdirectory (it's already downloaded as a dependency of ANTLR4 runtime)
add_subdirectory(vendor/antlr4/runtime/_deps/googletest-src)

# Enable testing
enable_testing()

# Placeholder for a C++ test executable
add_executable(expresso_cpp_tests tests/unit/parser/test_placeholder.cpp)
target_link_libraries(expresso_cpp_tests GTest::gtest_main)
