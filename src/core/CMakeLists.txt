# Build the core C17 evaluation logic as a static library
add_library(expresso_core STATIC
    value.c
    evaluator.c
    history.c # Add history.c here
)

# Include directories for core
target_include_directories(expresso_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

