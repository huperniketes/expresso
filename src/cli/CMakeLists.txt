## CMake for the CLI executable

# Create the CLI executable from the C main and the C++ interface wrapper
add_executable(expresso
    main.c
    cli_interface.cpp
)

# Link against project libraries
target_link_libraries(expresso PRIVATE
    expresso_core
    expresso_parser
)

# Ensure the CLI gets the parser include dirs and its own headers
target_include_directories(expresso PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/parser
)

set_target_properties(expresso PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
