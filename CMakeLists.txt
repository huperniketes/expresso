cmake_minimum_required(VERSION 3.31)
project(expresso VERSION 0.1.0 LANGUAGES C CXX)

# Default language standards (kept for tools that check these variables)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for source code
add_subdirectory(src/core)
add_subdirectory(src/cli)

# Build ANTLR4 C++ runtime (provides antlr4_static/antlr4_shared targets)
# Ensure ANTLR runtime is configured before targets that depend on it so
# targets like expresso_parser can link to antlr targets at configure time.
add_subdirectory(vendor/antlr4)

# Now add parser (depends on ANTLR targets being available)
add_subdirectory(src/parser)

# Build tests option
option(BUILD_TESTS "Build unit and integration tests" ON)

# Add GoogleTest as a subdirectory (it's already downloaded as a dependency of ANTLR4 runtime)
if(BUILD_TESTS)
	add_subdirectory(vendor/antlr4/runtime/_deps/googletest-src)
	enable_testing()

	# C integration test (placeholder)
	add_executable(test_interactive_session tests/integration/test_interactive_session.c)
	# This test will eventually link against the CLI executable
	# target_link_libraries(test_interactive_session PRIVATE expresso)
		target_link_libraries(test_interactive_session PRIVATE expresso_parser)
		add_test(NAME interactive_session COMMAND test_interactive_session)

	# C unit test executables linking against the core library
	add_executable(test_value tests/unit/core/test_value.c)
		target_link_libraries(test_value PRIVATE expresso_core expresso_parser)
	add_test(NAME test_value COMMAND test_value)

			# Compile this test as C++ because it includes and uses C++ parser types
			set(TEST_EVALUATOR_SRC ${CMAKE_SOURCE_DIR}/tests/unit/core/test_evaluator.c)
			set_source_files_properties(${TEST_EVALUATOR_SRC} PROPERTIES LANGUAGE CXX)
			add_executable(test_evaluator ${TEST_EVALUATOR_SRC})
			target_link_libraries(test_evaluator PRIVATE expresso_core expresso_parser)
	add_test(NAME test_evaluator COMMAND test_evaluator)

		add_executable(test_history tests/unit/core/test_history.c)
		target_link_libraries(test_history PRIVATE expresso_core expresso_parser)
	add_test(NAME test_history COMMAND test_history)

	# Placeholder for a C++ test executable that uses googletest
	add_executable(expresso_cpp_tests tests/unit/parser/test_placeholder.cpp)
	target_link_libraries(expresso_cpp_tests PRIVATE expresso_parser GTest::gtest_main)
	add_test(NAME expresso_cpp_tests COMMAND expresso_cpp_tests)
endif()
