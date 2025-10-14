# Feature Specification: Expresso: Enhanced CLI and Input Handling

**Feature Branch**: `002-the-user-is`  
**Created**: 2025-10-14  
**Status**: Draft  
**Input**: User description: "The user is prompted for input with \"expr>\". User input is ASCII-encoded and limited to 1023 bytes. Incomplete expressions and lines ending with a backslash are errors and reported to the user. Extraneous input after a complete expression is erroroneous and reported to the user. Expresso retains a history of the last 10 lines input, accessible by up- and down-arrow. Non-printable characters which don't affect the input line or commands are ignored, except for carriage-return or linefeed which ends input. Empty lines or input with only whitespace- or ignored non-printable characters are ignored, and the user is prompted again for input. Nested expressions (delimited by parentheses) are limited to 255 depth. Expresso accepts command-line arguments for files of expressions, or accepts input from stdin. Expresso accepts a \"-e\" flag for execute or non-interactive mode, and an \"-o\" flag to redirect output to a file. Expresso accepts either \"exit\", \"quit\", or ctrl-D as the command to terminate a session, whereupon it cleans up resources and terminates silently. Expresso terminates with an exit code reflecting the status of the last command executed: 0 for success, 1 for syntax error, 2 for evaluation error, 3 for division-by-zero, and so on."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Interactive Session with History (Priority: P1)

A user wants to interactively evaluate expressions, see a clear prompt, and easily recall previous inputs using command history.

**Why this priority**: This covers the primary interactive use case and enhances user experience significantly.

**Independent Test**: Can be tested by starting the CLI, entering several expressions, recalling them with arrow keys, and verifying the prompt and input limits.

**Acceptance Scenarios**:

1. **Given** the Expresso CLI is running, **When** the user types `1 + 1` and presses Enter, **Then** the tool prints `2` and displays the `expr>` prompt.
2. **Given** the Expresso CLI is running and several expressions have been entered, **When** the user presses the Up arrow key, **Then** the previous expression appears on the input line.
3. **Given** the Expresso CLI is running, **When** the user types an expression exceeding 1023 bytes, **Then** the tool reports an error like "Error: Input line too long" and prompts again.

---

### User Story 2 - Non-Interactive Evaluation (Priority: P2)

A user wants to evaluate expressions from a file or directly from a script without interactive prompts, and optionally redirect the output.

**Why this priority**: Supports automation and integration with other tools.

**Independent Test**: Can be tested by running `expresso -e "10 * 5"` or `expresso my_expressions.txt` and verifying the output.

**Acceptance Scenarios**:

1. **Given** a file `expressions.txt` contains `2 + 2`, **When** the user runs `expresso expressions.txt`, **Then** the tool prints `4` to stdout and exits with code 0.
2. **Given** the user runs `echo "3 * 3" | expresso -e`, **Then** the tool prints `9` to stdout and exits with code 0.
3. **Given** the user runs `expresso -e "4 / 2" -o output.txt`, **Then** the tool writes `2` to `output.txt` and exits with code 0.

---

### User Story 3 - Robust Error Handling and Controlled Exit (Priority: P3)

A user encounters various error conditions or wishes to terminate the session cleanly, expecting clear feedback and predictable behavior.

**Why this priority**: Ensures reliability, user trust, and proper resource management.

**Independent Test**: Can be tested by entering various invalid inputs, attempting different exit commands, and verifying error messages, resource cleanup, and exit codes.

**Acceptance Scenarios**:

1. **Given** the Expresso CLI is running, **When** the user types `1 +` and presses Enter, **Then** the tool reports an error like "Error: Incomplete expression" and prompts again.
2. **Given** the Expresso CLI is running, **When** the user types `exit` and presses Enter, **Then** the tool cleans up resources, terminates silently, and exits with code 0.
3. **Given** the Expresso CLI is running, **When** an evaluation error occurs (e.g., `1 / 0`), **Then** the tool reports an error like "Error: Division by zero" and exits with code 3.

---

### Edge Cases

- **Input Line Limit**: Input exceeding 1023 bytes results in an error.
- **Incomplete Expressions**: Lines ending with a backslash or otherwise syntactically incomplete expressions are reported as errors.
- **Extraneous Input**: Input after a complete expression on the same line is reported as an error.
- **Non-printable Characters**: Non-printable characters (excluding CR/LF) are ignored in interactive input.
- **Empty/Whitespace Input**: Empty lines or lines containing only whitespace/ignored non-printable characters are ignored, and the prompt is re-displayed.
- **Nested Expression Depth**: Expressions with nesting depth greater than 255 are reported as errors.
- **Invalid Command-Line Arguments**: Unrecognized flags or malformed arguments result in an error and usage message.
- **Resource Cleanup Failure**: System handles potential failures during resource cleanup on exit.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST prompt the user for input with the string "expr>".
- **FR-002**: The system MUST accept ASCII-encoded user input.
- **FR-003**: The system MUST limit interactive user input lines to 1023 bytes.
- **FR-004**: The system MUST report an error for incomplete expressions.
- **FR-005**: The system MUST report an error for lines ending with a backslash.
- **FR-006**: The system MUST report an error for extraneous input after a complete expression on the same line.
- **FR-007**: The system MUST retain a history of the last 10 input lines.
- **FR-008**: The system MUST allow users to access input history using up- and down-arrow keys.
- **FR-009**: The system MUST ignore non-printable characters in interactive input that do not affect the input line or commands (excluding carriage-return or linefeed).
- **FR-010**: The system MUST ignore empty lines or lines containing only whitespace or ignored non-printable characters, and re-display the prompt.
- **FR-011**: The system MUST limit nested expressions (delimited by parentheses) to a maximum depth of 255.
- **FR-012**: The system MUST accept command-line arguments specifying files containing expressions for evaluation.
- **FR-013**: The system MUST support a "-e" flag for execute or non-interactive mode, where expressions are evaluated without an interactive prompt.
- **FR-014**: The system MUST accept expressions from standard input (stdin) when not in execute mode.
- **FR-015**: The system MUST support an "-o" flag to redirect standard output to a specified file.
- **FR-016**: The system MUST terminate a session upon receiving "exit", "quit", or Ctrl-D.
- **FR-017**: The system MUST clean up all allocated resources upon termination.
- **FR-018**: The system MUST terminate silently (without printing messages) upon a clean exit.
- **FR-019**: The system MUST terminate with an exit code reflecting the status of the last command executed: 0 for success, 1 for syntax error, 2 for evaluation error, 3 for division-by-zero, and so on.

### Key Entities *(include if feature involves data)*

- **Expression**: A user-provided string representing the code to be evaluated.
- **Value**: The result of an evaluation. It can be an integer, float, character, or string. It is immutable. Floating-point numbers MUST be handled with standard `double` (64-bit) precision.
- **Input History**: A circular buffer storing the last 10 unique input lines.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: The CLI correctly displays the `expr>` prompt and processes user input according to all specified rules (encoding, limits, ignored characters).
- **SC-002**: The input history accurately stores and recalls the last 10 unique input lines via arrow keys.
- **SC-003**: All specified error conditions (input limit, incomplete, extraneous, nesting depth) are detected and reported with appropriate messages.
- **SC-004**: Non-interactive modes (`-e` flag, file input, stdin piping) function correctly, producing expected output and exit codes.
- **SC-005**: The CLI terminates cleanly and silently via all specified exit methods, with all resources deallocated and the correct exit code.
