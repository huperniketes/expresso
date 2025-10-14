# Checklist: Build & Integration (QA Gate)

**Purpose**: A comprehensive, rigorous checklist to validate the quality and completeness of requirements for the build system, dependency management, and C/C++ integration.
**Created**: 2025-10-14
**Feature**: [plan.md](../plan.md)

## Requirement Completeness

- [ ] CHK027 - Does the `quickstart.md` list the exact minimum required versions for all prerequisites (C++ Compiler, CMake, ANTLR4 JAR)? [Completeness, quickstart.md]
- [ ] CHK028 - Are all third-party dependencies (e.g., ANTLR4 C++ runtime) and their specific versions documented in the plan? [Completeness, Plan §Technical Context]
- [ ] CHK029 - Does the plan specify the required compiler flags for both release and debug builds for C and C++ code to ensure consistency? [Gap, Plan §Technical Context]
- [ ] CHK030 - Are requirements for handling platform-specific compilation differences (e.g., for Linux, macOS, and Windows) documented? [Gap, Plan §Target Platform]
- [ ] CHK031 - Is there a requirement to add all generated files (e.g., from ANTLR, CMake) to the project's `.gitignore` file? [Gap]

## Requirement Clarity

- [ ] CHK032 - Is the C/C++ integration strategy (static library with `extern "C"` wrapper) documented with enough detail in `research.md` for a developer to implement it without ambiguity? [Clarity, research.md]
- [ ] CHK033 - Are the steps to generate the ANTLR parser in `quickstart.md` completely unambiguous, including the exact command, arguments, and expected working directory? [Clarity, quickstart.md]
- [ ] CHK034 - Does the `plan.md` provide a clear diagram or description of the final linking process (i.e., which libraries link into the final executable)? [Gap, Plan §Project Structure]
- [ ] CHK035 - Are the names of the final build artifacts (e.g., `libexpresso-core.a`, `expresso` executable) explicitly defined in the plan? [Gap]

## Consistency

- [ ] CHK036 - Are the directory paths used in the `quickstart.md` build instructions consistent with the `Project Structure` defined in `plan.md`? [Consistency]
- [ ] CHK037 - Is the C++ language version (`C++17`) specified in `plan.md` consistent with the ANTLR generation command and CMake configuration? [Consistency, Plan §Technical Context]

## Verification & Testability

- [ ] CHK038 - Does the plan include a requirement for a dedicated integration test (or 'smoke test') that explicitly verifies the C core can successfully call the C++ parser and receive data? [Gap]
- [ ] CHK039 - Is there a requirement for the build system to fail if any of the required tools (CMake, ANTLR4, etc.) are not found? [Completeness]
