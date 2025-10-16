# Optionally auto-generate parser sources from the grammar using ANTLR
option(AUTO_GENERATE_PARSER "Automatically generate parser sources from Expresso.g4 if missing" ON)
set(ANTLR_JAR_LOCATION "" CACHE FILEPATH "Path to antlr-4.*-complete.jar used to generate C++ parser sources")

set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
set(GENERATED_FILES
  ${GENERATED_DIR}/ExpressoLexer.cpp
  ${GENERATED_DIR}/ExpressoLexer.h
  ${GENERATED_DIR}/ExpressoParser.cpp
  ${GENERATED_DIR}/ExpressoParser.h
  ${GENERATED_DIR}/ExpressoVisitor.cpp
  ${GENERATED_DIR}/ExpressoVisitor.h
  ${GENERATED_DIR}/ExpressoBaseVisitor.cpp
  ${GENERATED_DIR}/ExpressoBaseVisitor.h
  ${GENERATED_DIR}/Expresso.tokens
)

# If generated parser sources are not present, require the user to either
# provide an ANTLR jar and enable AUTO_GENERATE_PARSER or run ANTLR manually.
if(NOT EXISTS "${GENERATED_DIR}/ExpressoParser.cpp")
  if(NOT (AUTO_GENERATE_PARSER AND ANTLR_JAR_LOCATION))
    message(FATAL_ERROR "ANTLR generated parser sources not found in ${GENERATED_DIR}.\n"
      "Either set -DANTLR_JAR_LOCATION=/path/to/antlr-4.x-complete.jar \n"
      "and -DAUTO_GENERATE_PARSER=ON to generate them at build-time, or run ANTLR manually to produce the files into ${GENERATED_DIR}.")
  endif()
endif()

if(AUTO_GENERATE_PARSER)
  # If generated parser sources are missing, add a custom command to run the ANTLR jar
  if(NOT EXISTS ${GENERATED_DIR}/ExpressoParser.cpp)
    if(NOT ANTLR_JAR_LOCATION)
      message(STATUS "ANTLR_JAR_LOCATION not set; parser generation skipped. To enable auto-generation set -DANTLR_JAR_LOCATION=/path/to/antlr-4.x-complete.jar or set AUTO_GENERATE_PARSER=OFF")
    else()
      # Ensure output directory exists at generate time
      file(MAKE_DIRECTORY ${GENERATED_DIR})

      add_custom_command(
        OUTPUT ${GENERATED_FILES}
        COMMAND ${CMAKE_COMMAND} -E echo "Generating ANTLR C++ parser sources into ${GENERATED_DIR}..."
        COMMAND java -jar "${ANTLR_JAR_LOCATION}" -Dlanguage=Cpp -visitor -no-listener -o "${GENERATED_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Expresso.g4"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Expresso.g4"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running ANTLR to generate parser source files"
        VERBATIM
      )

      add_custom_target(generate_parser DEPENDS ${GENERATED_FILES})
    endif()
  endif()
endif()

# Build the C++ parser wrapper as a static library
add_library(expresso_parser STATIC
    parser_wrapper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/generated/ExpressoLexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/generated/ExpressoParser.cpp
)

# If we created a generate_parser target, ensure the parser library depends on it
if(TARGET generate_parser)
  add_dependencies(expresso_parser generate_parser)
endif()

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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated>
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
