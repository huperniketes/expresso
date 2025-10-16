# Build the core C17 evaluation logic as a static library
add_library(expresso_core STATIC
    value.c
    evaluator.c
    history.c
)

# Require C17 for the core library
target_compile_features(expresso_core PUBLIC c_std_17)

# Export the include directory for consumers
target_include_directories(expresso_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Build as position-independent to be linkable into shared libraries if needed
set_target_properties(expresso_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Install rules for expresso_core
install(TARGETS expresso_core
    EXPORT expresso-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

