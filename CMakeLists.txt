cmake_minimum_required(VERSION 3.31)
project(expresso VERSION 0.1.0 LANGUAGES C CXX)

# Option to auto-fetch ANTLR jar when not provided by the user. Default OFF.
option(AUTO_FETCH_ANTLR_JAR "Automatically download the ANTLR complete jar to build_tools/antlr if ANTLR_JAR_LOCATION is not set" OFF)

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
	# GoogleTest is provided by the ANTLR runtime's FetchContent population.
	# Do not add_subdirectory here to avoid duplicate target definitions.
	enable_testing()

	# C integration test (placeholder)
	add_executable(test_interactive_session tests/integration/test_interactive_session.c)
	# This test will eventually link against the CLI executable
	target_link_libraries(test_interactive_session PRIVATE expresso)
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

	add_executable(test_non_interactive tests/integration/test_non_interactive.c)
	target_link_libraries(test_non_interactive PRIVATE expresso)
	target_include_directories(test_non_interactive PRIVATE tests/unit/core)
	add_test(NAME test_non_interactive COMMAND test_non_interactive)
endif()

# Installation and export configuration
include(CMakePackageConfigHelpers)

set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")

if(TARGET antlr4_static)
  install(TARGETS antlr4_static EXPORT expresso-targets
	  ARCHIVE DESTINATION lib
	  LIBRARY DESTINATION lib
	  RUNTIME DESTINATION bin)
elseif(TARGET antlr4_shared)
  install(TARGETS antlr4_shared EXPORT expresso-targets
	  ARCHIVE DESTINATION lib
	  LIBRARY DESTINATION lib
	  RUNTIME DESTINATION bin)
endif()

install(EXPORT expresso-targets
	FILE ExpressoTargets.cmake
	NAMESPACE Expresso::
	DESTINATION lib/cmake/Expresso
)

## CPack packaging settings
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "expresso")
set(CPACK_PACKAGE_VENDOR "Expresso Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Expresso — expression evaluator and CLI")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ;ZIP;ProductBuild")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# macOS package settings (ProductBuild)
set(CPACK_PRODUCTBUILD_IDENTIFIER "com.expresso.app")
set(CPACK_PRODUCTBUILD_PACKAGE_IDENTIFIER "com.expresso.pkg")
set(CPACK_PRODUCTBUILD_DISTRIBUTION "")

# Ensure component based install for macOS packages if needed
set(CPACK_COMPONENTS_ALL runtime)

include(CPack)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ExpressoConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ExpressoConfig.cmake
  INSTALL_DESTINATION lib/cmake/Expresso
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ExpressoConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ExpressoConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/ExpressoConfigVersion.cmake
	DESTINATION lib/cmake/Expresso)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)

## Note: targets are installed and exported by their respective subdirectories
## (src/core, src/parser, src/cli). Do not repeat install(TARGETS ...) here
## or CMake will include targets more than once in the export set.
