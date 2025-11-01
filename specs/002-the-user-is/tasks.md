# Tasks: Expresso: Enhanced CLI and Input Handling

**Input**: Design documents from `/specs/002-the-user-is/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Test tasks are included as TDD is required by the project constitution.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`
- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions
- Paths are based on the `plan.md` structure: `src/`, `tests/` at repository root.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure.

- [X] T001 Create the project directory structure defined in `plan.md`: `src/cli`, `src/core`, `src/parser/generated`, `tests/integration`, `tests/unit/core`, `tests/unit/parser`, `vendor/antlr4`.
- [X] T002 Initialize the CMake build system (`CMakeLists.txt`) to handle the C, C++, and ANTLR components.
- [X] T003 [P] Add the ANTLR4 C++ runtime source to `vendor/antlr4`.
- [X] T004 [P] Configure GoogleTest framework for C++ tests within the CMake build system.
- [X] T005 [P] Create the custom `assert.h` framework for C tests in `tests/unit/core`.

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented.

- [X] T006 Define the Expresso grammar in `src/parser/Expresso.g4` based on `grammar-spec.md`.
- [X] T007 Generate the C++ ANTLR parser code into `src/parser/generated/` from `Expresso.g4`.
- [X] T008 [P] Implement the C-compatible wrapper for the C++ parser as a static library (`src/parser/parser_wrapper.cpp`, `src/parser/parser_wrapper.h`).
- [X] T009 [P] Implement the core `Value` variant type in `src/core/value.h` and `src/core/value.c` as defined in `data-model.md`.
- [X] T010 [P] Test [US1,US2,US3]: Write unit tests for the `Value` type in `tests/unit/core/test_value.c`.
- [X] T011 Implement the basic evaluator structure in `src/core/evaluator.h` and `src/core/evaluator.c`, including a placeholder evaluation function.
- [X] T012 Test [US1,US2,US3]: Write a basic unit test for the placeholder evaluator in `tests/unit/core/test_evaluator.c`.

**Checkpoint**: Foundation ready - user story implementation can now begin.

---

## Phase 3: User Story 1 - Interactive Session with History (Priority: P1) 🎯 MVP

**Goal**: A user can interactively evaluate expressions, see a clear prompt, and easily recall previous inputs.

**Independent Test**: Start the CLI, enter expressions, use arrow keys to recall history, and verify prompts and input limits.

### Tests for User Story 1 ⚠️

- [X] T013 [P] [US1] Test: Write an integration test in `tests/integration/test_interactive_session.c` to verify the `expr>` prompt, input handling, and correct output for a simple expression.
- [X] T014 [P] [US1] Test: Write a unit test in `tests/unit/core/test_history.c` for the input history buffer (add, recall, clear).

### Implementation for User Story 1

- [X] T015 [US1] Implement the `Input History` circular buffer in `src/core/history.h` and `src/core/history.c`.
- [X] T016 [US1] Implement the main input loop in `src/cli/main.c` to display the "expr>" prompt (FR-001) and read user input.
- [X] T017 [US1] Integrate the history buffer with the input loop to allow up/down arrow history navigation (FR-008).
- [X] T018 [US1] Implement the `!history`, `!clear`, and `!n` meta-commands in the input loop (FR-007, FR-009 from cli-interface).
- [X] T019 [US1] Connect the input loop to the evaluator: pass valid expressions to the evaluator and print the result.
- [X] T020 [US1] Implement basic evaluation for simple arithmetic (`+`, `*`) in `src/core/evaluator.c`.

**Checkpoint**: User Story 1 is fully functional and testable independently.

---

## Phase 4: User Story 2 - Non-Interactive Evaluation (Priority: P2)

**Goal**: A user can evaluate expressions from a file or script without interactive prompts.

**Independent Test**: Run `expresso -e "10 * 5"` or `expresso my_expressions.txt` and verify the output.

### Tests for User Story 2 ⚠️

- [X] T021 [P] [US2] Test: Write an integration test in `tests/integration/test_non_interactive.c` to verify the `-e` flag (FR-013).
- [ ] T022 [P] [US2] Test: Write an integration test in `tests/integration/test_non_interactive.c` to verify file-based input (FR-012).
- [ ] T023 [P] [US2] Test: Write an integration test in `tests/integration/test_non_interactive.c` to verify stdin piping (FR-014).
- [ ] T024 [P] [US2] Test: Write an integration test in `tests/integration/test_non_interactive.c` to verify the `-o` output redirection flag (FR-015).

### Implementation for User Story 2

- [ ] T025 [US2] Implement command-line argument parsing in `src/cli/main.c` to handle `-e`, `-o`, and file path arguments.
- [X] T026 [US2] Implement the logic for non-interactive mode: if `-e` is present, evaluate the expression and exit.
- [ ] T027 [US2] Implement the logic for file input mode: if a file path is provided, read and evaluate expressions from the file.
- [ ] T028 [US2] Implement the logic for stdin piping: if no interactive terminal is detected, read and evaluate expressions from stdin.
- [ ] T029 [US2] Implement output redirection to a file when the `-o` flag is used.

**Checkpoint**: User Story 2 is fully functional and testable independently.

---

## Phase 5: User Story 3 - Robust Error Handling and Controlled Exit (Priority: P3)

**Goal**: The user receives clear feedback on errors and can terminate the session cleanly.

**Independent Test**: Enter invalid inputs, use exit commands, and verify error messages and exit codes.

### Tests for User Story 3 ⚠️

- [ ] T030 [P] [US3] Test: Write unit tests in `tests/unit/core/test_errors.c` for all specified error types (Syntax, Type, Runtime).
- [ ] T031 [P] [US3] Test: Write an integration test in `tests/integration/test_exit.c` to verify that `exit`, `quit`, and Ctrl-D terminate the application with exit code 0 (FR-016, FR-018, FR-019).

### Implementation for User Story 3

- [ ] T032 [US3] Implement the `Error` types from `data-model.md` in `src/core/errors.h`.
- [ ] T033 [US3] Integrate error reporting from the parser and evaluator, printing messages to stderr prefixed with "Error: " (FR-020).
- [ ] T034 [US3] Implement handling for all specified error conditions (incomplete expression, extraneous input, nesting depth, etc.) as defined in `spec.md`.
- [ ] T035 [US3] Implement the `exit` and `quit` commands and handle Ctrl-D to terminate the session.
- [ ] T036 [US3] Implement resource cleanup logic to be called on exit (FR-017).
- [ ] T037 [US3] Ensure the application returns the correct exit code based on the result of the last operation (FR-019).

**Checkpoint**: All user stories are now functionally complete.

---

## Phase N: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories.

- [ ] T038 [P] [US1] Add welcome message: "Welcome to Expresso v1.0..."
- [ ] T039 [P] [US1] Implement output truncation for long results (FR-021).
- [ ] T040 [P] [US1] Add color to CLI output (green for results, red for errors) with a plain text fallback (NFR-005, NFR-006).
- [ ] T041 [P] Documentation: Update `README.md` with build and usage instructions from `quickstart.md`.
- [ ] T042 [P] Refactor and clean up code across all modules.
- [ ] T043 Run `quickstart.md` validation steps to ensure the build process is correct.
- [ ] T044 [P] Create benchmark tests in `tests/performance/` to measure evaluation latency and memory usage (NFR-001, NFR-002).
- [ ] T045 Run and document benchmark results to verify performance goals are met.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2)
- **User Story 2 (P2)**: Can start after Foundational (Phase 2)
- **User Story 3 (P3)**: Can start after Foundational (Phase 2)

### Within Each User Story

- Tests MUST be written and FAIL before implementation.
- Models/core types before services/evaluation logic.
- Core implementation before integration.

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently.

### Incremental Delivery

1. Complete Setup + Foundational.
2. Add User Story 1 → Test independently.
3. Add User Story 2 → Test independently.
4. Add User Story 3 → Test independently.
