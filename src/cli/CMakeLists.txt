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

## Require C and C++ standards for this target
target_compile_features(expresso PUBLIC c_std_17 cxx_std_17)

# The parser library already exports its include directories, so only expose local headers
target_include_directories(expresso PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(expresso PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ENABLE_EXPORTS ON
)

# Install the CLI executable
install(TARGETS expresso
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/cli
    FILES_MATCHING PATTERN "*.h"
)
