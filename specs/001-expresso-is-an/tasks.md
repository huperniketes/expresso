# Task Plan: Expresso: Interactive Expression Evaluator

**Branch**: `001-expresso-is-an` | **Date**: 2025-10-14 | **Plan**: [plan.md](plan.md)

This document breaks down the implementation of the Expresso CLI into actionable, dependency-ordered tasks. The project follows a strict Test-Driven Development (TDD) approach.

## Implementation Strategy

The project is broken down into phases. We will start with project setup and foundational code (parser, core types). Then, each User Story from the specification will be implemented in its own phase, starting with the highest priority (P1). Each phase is independently testable, allowing for incremental delivery. The MVP is the completion of User Story 1.

---

## Phase 1: Project Setup

**Goal**: Create the basic directory structure and build system configuration.

- `[ ]` **T001**: Create the project directory structure as defined in `plan.md`: `src/cli`, `src/core`, `src/parser/generated`, `tests/unit/core`, `tests/unit/parser`, `tests/integration`, `vendor/`.
- `[ ]` **T002**: Add the ANTLR4 C++ runtime source files to the `vendor/antlr4` directory.
- `[ ]` **T003**: Create the root `CMakeLists.txt` file to define the project, subdirectories, and CTest configuration.

## Phase 2: Foundational - Parser & Core Types

**Goal**: Set up the core components required for any expression evaluation.

- `[ ]` **T004**: Define the expression grammar in `src/parser/Expresso.g4`. This grammar should include rules for all specified operators and data types. [P]
- `[ ]` **T005**: Create the `CMakeLists.txt` for the `src/parser` directory. Configure it to use ANTLR to generate the C++ parser and lexer from `Expresso.g4` into the `src/parser/generated` directory. [P]
- `[ ]` **T006**: Define the core data structures in C, including the `Value` variant (`int64`, `double`, `char`, `string`) and the Abstract Syntax Tree (AST) nodes, in `src/core/types.h`. [P]
- `[ ]` **T007**: Implement the C-to-C++ wrapper for the parser. Create `src/parser/parser_wrapper.h` (with `extern "C"`) and `src/parser/parser_wrapper.cpp` to expose a simple `parse_expression(const char* input)` function that returns a pointer to the root of the AST.
- `[ ]` **T008**: Set up the GoogleTest framework for C++ tests and the custom assert framework for C tests. Configure CMake in the `tests/` directory to build and link the test suites.

**Checkpoint**: The project structure is in place, and the build system can generate the parser and compile the empty core library and parser wrapper.

---

## Phase 3: [US1] Basic Arithmetic Evaluation

**Goal**: Implement the core evaluation logic for basic arithmetic, enabling the MVP.
**Independent Test**: The CLI can correctly evaluate expressions like `(10 + 2) * 5` and print the result.

- `[ ]` **T009**: [Test] Write unit tests in `tests/unit/core/` for the evaluator. Start with tests for basic arithmetic (`+`, `-`, `*`, `/`) and parentheses using a mock AST. [P]
- `[ ]` **T010**: [Test] Write a simple integration test in `tests/integration/` that runs the `expresso` executable, pipes `"10 + 5"` to stdin, and asserts that `"15"` is printed to stdout. [P]
- `[ ]` **T011**: Implement the AST visitor/evaluator in the `src/core` library. Create `evaluator.c` and `evaluator.h`. The initial version should only handle the operators and structures required for basic arithmetic.
- `[ ]` **T012**: Implement the main application loop in `src/cli/main.c`. It should read a line from stdin, pass it to the `parse_expression` wrapper, send the resulting AST to the evaluator, and print the final `Value` to stdout.

**Checkpoint**: User Story 1 is complete. The `expresso` CLI is a functional, albeit basic, calculator.

---

## Phase 4: [US2] Complex Expression Evaluation

**Goal**: Extend the evaluator to support all data types and operators.
**Independent Test**: The CLI can correctly evaluate expressions like `(1 << 2) + 5` and `"hello" + " world"`.

- `[ ]` **T013**: [Test] Write unit tests in `tests/unit/core/` for logical (`&&`, `||`, `!`), comparison (`==`, `!=`, `<`, `>`), bitwise (`&`, `|`, `^`, `~`, `<<`, `>>`), and string concatenation operators. [P]
- `[ ]` **T014**: Extend the evaluator in `src/core/evaluator.c` to handle the new operators and data types defined in the tests.

**Checkpoint**: User Story 2 is complete. The evaluator now supports the full range of specified language features.

---

## Phase 5: [US3] Invalid Expression Handling

**Goal**: Implement robust, user-friendly error reporting.
**Independent Test**: The CLI prints a clear error message to stderr for invalid input like `5 +` or `a = 10`.

- `[ ]` **T015**: [Test] Write unit tests in `tests/unit/parser/` that pass invalid expressions to the parser and assert that the correct error information is returned via the C wrapper. [P]
- `[ ]` **T016**: [Test] Write integration tests in `tests/integration/` that pipe invalid expressions to the CLI and assert that the correct error messages are printed to stderr. [P]
- `[ ]` **T017**: Implement the error propagation logic. Modify the parser wrapper and evaluator to return error objects instead of crashing. Update the CLI in `src/cli/main.c` to check for errors and print the user-facing messages to stderr.

**Checkpoint**: User Story 3 is complete. The CLI is now robust against user syntax errors.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Goal**: Implement final requirements from the specification clarifications.

- `[ ]` **T018**: [Test] Write an integration test for the `exit` command. [P]
- `[ ]` **T019**: Implement the `exit` command logic in `src/cli/main.c`. [P]
- `[ ]` **T020**: [Test] Write unit tests for division-by-zero and numeric overflow scenarios in `tests/unit/core/`. [P]
- `[ ]` **T021**: Add checks in `src/core/evaluator.c` to detect division-by-zero and numeric overflow and return specific errors.
- `[ ]` **T022**: Implement the internal error logging mechanism to write to `expresso-error.log` as defined in `spec.md §NFR-001`.
- `[ ]` **T023**: [Test] Write an integration test that pipes non-printable characters (e.g., ASCII control codes) to the CLI and asserts that they are handled gracefully (e.g., ignored or reported as an error). [P]
- `[ ]` **T024**: Implement logic in the CLI input handling (`src/cli/main.c`) to filter out or report errors on non-printable characters before they are sent to the parser.
- `[ ]` **T025**: [Test] Write a unit test that attempts to parse an expression with nesting depth greater than a defined limit (e.g., 100) and asserts that a specific 'nesting depth exceeded' error is returned. [P]
- `[ ]` **T026**: Implement a check in the parser or evaluator to track expression nesting depth and return an error if it exceeds a predefined limit.

---

## Dependencies & Parallel Execution

- **Story Dependencies**: US1 is the foundational MVP. US2 and US3 can be worked on in parallel after US1 is complete.
- **Parallel Tasks**: Tasks marked with `[P]` can be worked on in parallel within their phase.
  - **Phase 2**: `T004`, `T005`, `T006` can be started simultaneously.
  - **Phase 3**: `T009` and `T010` (tests) can be written in parallel.
  - **And so on for other phases...**
