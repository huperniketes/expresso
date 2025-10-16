# Build the C++ parser wrapper as a static library
add_library(expresso_parser STATIC
    parser_wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/src/parser/ExpressoLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/src/parser/ExpressoParser.cpp
)

# Require C++17 for the parser library
target_compile_features(expresso_parser PUBLIC cxx_std_17)

# Prefer to link against the ANTLR target if it exists; otherwise fall back to antlr4_static
if(TARGET antlr4_static)
  # Link ANTLR runtime privately so the build links correctly but the
  # exported package doesn't require exporting the ANTLR runtime target.
  # Consumers that need the shared runtime should find and link ANTLR
  # separately if required.
  target_link_libraries(expresso_parser PRIVATE antlr4_static)
elseif(TARGET antlr4_shared)
  target_link_libraries(expresso_parser PRIVATE antlr4_shared)
else()
  # If the ANTLR runtime isn't available as a target, the top-level CMake should add it.
  message(STATUS "ANTLR runtime target not found at configure time; ensure vendor/antlr4 is added.")
endif()

# Include directories for generated files (exposed to consumers)
target_include_directories(expresso_parser PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/src/parser>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)

# Mark the ANTLR4 runtime headers as system includes to suppress warnings from third-party code
target_include_directories(expresso_parser SYSTEM PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/antlr4/runtime/src>
)

# Build as position-independent code to allow linking into shared libs
set_target_properties(expresso_parser PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Provide an alias for downstream convenience
add_library(expresso::parser ALIAS expresso_parser)

# Install rules for parser library and headers
install(TARGETS expresso_parser
  EXPORT expresso-targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION include/parser
  FILES_MATCHING PATTERN "*.h"
)
