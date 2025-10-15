# Build the C++ parser wrapper as a static library
add_library(expresso_parser STATIC
    parser_wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/src/parser/ExpressoLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/src/parser/ExpressoParser.cpp
)

# Link against ANTLR4 runtime
target_link_libraries(expresso_parser PRIVATE
    antlr4_static # Link against the static ANTLR4 runtime library
)

# Include directories for generated files and ANTLR4 runtime
target_include_directories(expresso_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/src/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/src
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

# Mark the ANTLR4 runtime headers as system includes to suppress warnings from third-party code
target_include_directories(expresso_parser SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/antlr4/runtime/src
)
