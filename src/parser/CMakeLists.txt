# Optionally auto-generate parser sources from the grammar using ANTLR
option(AUTO_GENERATE_PARSER "Automatically generate parser sources from Expresso.g4 if missing" ON)
set(ANTLR_JAR_LOCATION "" CACHE FILEPATH "Path to antlr-4.*-complete.jar used to generate C++ parser sources")

# Optionally auto-fetch the ANTLR jar if the user hasn't provided a path
option(AUTO_FETCH_ANTLR_JAR "Automatically download the matching ANTLR complete jar when ANTLR_JAR_LOCATION is not supplied" OFF)

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
  # Allow either an explicit ANTLR_JAR_LOCATION or AUTO_FETCH_ANTLR_JAR to satisfy
  # the requirement for generating parser sources at configure time.
  if(NOT (AUTO_GENERATE_PARSER AND (ANTLR_JAR_LOCATION OR AUTO_FETCH_ANTLR_JAR)))
    message(FATAL_ERROR "ANTLR generated parser sources not found in ${GENERATED_DIR}.\n"
      "Either set -DANTLR_JAR_LOCATION=/path/to/antlr-4.x-complete.jar \n"
      "and -DAUTO_GENERATE_PARSER=ON to generate them at build-time, or run ANTLR manually to produce the files into ${GENERATED_DIR}.\n"
      "Alternatively set -DAUTO_FETCH_ANTLR_JAR=ON to have CMake try to download the matching ANTLR jar for you.")
  endif()
endif()

if(AUTO_GENERATE_PARSER)
  # If generated parser sources are missing, add a custom command to run the ANTLR jar
  if(NOT EXISTS ${GENERATED_DIR}/ExpressoParser.cpp)
    # If user didn't set ANTLR_JAR_LOCATION but asked to auto-fetch, try to download it
    if(NOT ANTLR_JAR_LOCATION)
      if(AUTO_FETCH_ANTLR_JAR)
        set(_antlr_ver_file "${CMAKE_SOURCE_DIR}/vendor/antlr4/VERSION")
        if(EXISTS ${_antlr_ver_file})
          file(READ ${_antlr_ver_file} _antlr_ver_raw)
          string(STRIP "${_antlr_ver_raw}" _antlr_ver)
          set(_antlr_jar_name "antlr-${_antlr_ver}-complete.jar")
          set(_antlr_jar_url "https://www.antlr.org/download/${_antlr_jar_name}")
          set(_antlr_dest_dir "${CMAKE_SOURCE_DIR}/build_tools/antlr")
          file(MAKE_DIRECTORY "${_antlr_dest_dir}")
          set(_antlr_dest_path "${_antlr_dest_dir}/${_antlr_jar_name}")
          if(NOT EXISTS "${_antlr_dest_path}")
            message(STATUS "AUTO_FETCH_ANTLR_JAR=ON: downloading ${_antlr_jar_name} to ${_antlr_dest_dir}...")
            file(DOWNLOAD "${_antlr_jar_url}" "${_antlr_dest_path}" SHOW_PROGRESS)
            if(NOT EXISTS "${_antlr_dest_path}")
              message(WARNING "Failed to download ANTLR jar from ${_antlr_jar_url}. Parser generation will be skipped unless you provide -DANTLR_JAR_LOCATION=/path/to/${_antlr_jar_name}.")
            else()
              set(ANTLR_JAR_LOCATION "${_antlr_dest_path}" CACHE FILEPATH "Path to antlr jar" FORCE)
              message(STATUS "Downloaded ANTLR jar to ${_antlr_dest_path}")
              # Run ANTLR at configure time so generated sources exist immediately
              if(AUTO_GENERATE_PARSER)
                file(MAKE_DIRECTORY "${GENERATED_DIR}")
                message(STATUS "Running ANTLR to generate parser sources at configure time...")
                execute_process(
                  COMMAND java -jar "${ANTLR_JAR_LOCATION}" -Dlanguage=Cpp -visitor -no-listener -o "${GENERATED_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Expresso.g4"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  RESULT_VARIABLE _antlr_exec_res
                  OUTPUT_VARIABLE _antlr_exec_out
                  ERROR_VARIABLE _antlr_exec_err
                )
                if(NOT _antlr_exec_res EQUAL 0)
                  message(FATAL_ERROR "ANTLR generation failed at configure time: ${_antlr_exec_err}")
                else()
                  message(STATUS "ANTLR generation completed at configure time")
                endif()
              endif()
            endif()
          else()
            set(ANTLR_JAR_LOCATION "${_antlr_dest_path}" CACHE FILEPATH "Path to antlr jar" FORCE)
            message(STATUS "Using existing ANTLR jar at ${_antlr_dest_path}")
            if(AUTO_GENERATE_PARSER)
              file(MAKE_DIRECTORY "${GENERATED_DIR}")
              message(STATUS "Running ANTLR to generate parser sources at configure time using existing jar...")
              execute_process(
                COMMAND java -jar "${ANTLR_JAR_LOCATION}" -Dlanguage=Cpp -visitor -no-listener -o "${GENERATED_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Expresso.g4"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                RESULT_VARIABLE _antlr_exec_res
                OUTPUT_VARIABLE _antlr_exec_out
                ERROR_VARIABLE _antlr_exec_err
              )
              if(NOT _antlr_exec_res EQUAL 0)
                message(FATAL_ERROR "ANTLR generation failed at configure time: ${_antlr_exec_err}")
              else()
                message(STATUS "ANTLR generation completed at configure time")
              endif()
            endif()
          endif()
        else()
          message(STATUS "AUTO_FETCH_ANTLR_JAR requested but ${_antlr_ver_file} not found; cannot determine ANTLR version. Set -DANTLR_JAR_LOCATION manually.")
        endif()
      else()
        message(STATUS "ANTLR_JAR_LOCATION not set; parser generation skipped. To enable auto-generation set -DANTLR_JAR_LOCATION=/path/to/antlr-4.x-complete.jar or set AUTO_GENERATE_PARSER=OFF")
      endif()
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
