# Feature Specification: Expresso: Enhanced CLI and Input Handling

**Feature Branch**: `002-the-user-is`  
**Created**: 2025-10-14  
**Status**: Draft  
**Input**: User description: "The user is prompted for input with \"expr>\". User input is ASCII-encoded and limited to 1023 bytes. Incomplete expressions and lines ending with a backslash are errors and reported to the user. Extraneous input after a complete expression is erroroneous and reported to the user. Expresso retains a history of the last 10 lines input, accessible by up- and down-arrow. Non-printable characters which don't affect the input line or commands are ignored, except for carriage-return or linefeed which ends input. Empty lines or input with only whitespace- or ignored non-printable characters are ignored, and the user is prompted again for input. Nested expressions (delimited by parentheses) are limited to 255 depth. Expresso accepts command-line arguments for files of expressions, or accepts input from stdin. Expresso accepts a \"-e\" flag for execute or non-interactive mode, and an \"-o\" flag to redirect output to a file. Expresso accepts either \"exit\", \"quit\", or ctrl-D as the command to terminate a session, whereupon it cleans up resources and terminates silently. Expresso terminates with an exit code reflecting the status of the last command executed: 0 for success, 1 for syntax error, 2 for evaluation error, 3 for division-by-zero, and so on."

## System Architecture
### High-Level Overview
Expresso is structured as a modular CLI application:
- **Frontend**: User input loop (custom readline-like for history/navigation).
- **Parser**: Generated lexer/parser.
- **Evaluator**: Accepts parser output, evaluating it to compute results.
- **Output Handler**: Textualize results/errors.
- **Runtime**: Sandboxed execution ensuring no side effects.

Data Flow:
1. Read input → Classify (meta-command or expression).
2. For expressions: Lex → Parse → Evaluate → Textualize → Print.
3. For meta-commands: Execute internally → Print.

### Components
| Component | Description |
|-----------|-------------|
| Input Manager | Handles prompts, history (last 10 entries), arrow navigation. |
| Classifier | Checks if input starts with `!` for meta-commands or is treated as an expression. |
| Parser | Handles Expresso grammar. Rejects assignments/pointers early. |
| Evaluator | Handles parser output, evaluating it for results and errors; enforces immutability (e.g., + creates new string); handle type promotion (int→float); dynamic string allocation (strdup). |
| Error Handler | Collects syntax/type/runtime errors; provides position-aware messages. |
| Help/System Manager | Handles `!help`, `!history`, `!n`, `!clear`. |
| Output Formatter | Pretty-prints values (e.g., quote strings, hex for chars if needed). Colored CLI  (ANSI escape codes: green for results, red for errors; fallback to plain). |

### Deployment Diagram
- Single executable: `expresso`.
- No server; local execution.

## Data Model
### Value Representation
All values are immutable

- Type Coercion Rules:
  - **Core Principle**: There must be NO Undefined Behavior in the Expresso language and runtime. Any operation that would be undefined in C MUST be detected prior to execution and reported as a runtime error.
  - **Default Arithmetic Conversions**: For all binary arithmetic operations where behavior is not otherwise explicitly defined, Expresso adopts the 'usual arithmetic conversions' as defined by the C17 standard to determine the result type.
  - **String Operations**:
    - `String + <any>`: The right-hand operand is converted to its string representation and concatenated. This is the only non-comparison operator defined for a left-hand `String` operand.
    - `<numeric type> <op> String`: Where `<op>` is any arithmetic, logical, bitwise, or comparison operator, the runtime will attempt to parse the `String` operand as a number.
      - If the parse is successful, the resulting numeric value is used in the operation, with types promoted as per the Default Arithmetic Conversions.
      - If the parse is invalid, a runtime error is reported and evaluation ends.
  - **Bitwise Operations**: Bitwise operators are only valid for `int` and `char` types. A `char` operand is promoted to an `int` before the operation.

### Error Types
- SyntaxError: Struct with line, col, msg.
- TypeError: Position, msg.
- RuntimeError: Msg only.

## Expresso Language Specification
### Grammar

The Expresso grammar is defined in grammar-spec.md.

## Functional Specifications
### Core Loop
- Prompt: `expr> `.
- On Enter:
  - Empty/WS: Reprompt.
  - !command: Dispatch.
  - Else: Parse → Eval → Print.
- Exit: `!exit`, `!quit`, or Ctrl+D.

### Meta-Commands
| Command | Behavior |
|---------|----------|
| !help | Print a help table including: command-line arguments (`-e`, `-o`), available meta-commands, a list of supported operators by precedence, data types (integer, float, string, char), and a usage example like "1+2*3 → 7". |
| !history | List the last 10 unique input lines from history, formatted according to FR-008. |
| !n | Re-inputs the expression from history at the 1-based index `n`. If `n` is out of bounds (less than 1 or greater than the number of items in history), the system MUST print an error "Error: history index out of bounds" and re-prompt the user. |
| !clear | Clear the input history and print the message "Session history cleared." to stdout. |

### Evaluation Rules
- Support nested depth ≤255 (recursion limit with stack check).
- String limit: 9,999 characters.

#### Bitwise Shift Behavior
The behavior for invalid shift operations is explicitly defined to avoid undefined behavior:
- **Negative Shift Amount**: If the shift amount is negative, the direction of the shift is reversed, and the absolute value of the amount is used. For example, `x << -2` is equivalent to `x >> 2`, and `x >> -3` is equivalent to `x << 3`.
- **Shift Amount Greater Than or Equal to Bit-Width**:
  - For a left shift (`<<`), the result is always `0`.
  - For a right shift (`>>`) on a signed negative number, the result is `-1` (arithmetic shift).
  - For a right shift (`>>`) on a positive or unsigned number, the result is `0`.

#### Floating-Point Behavior
- **Special Values**: Any arithmetic operation that results in `NaN` (Not a Number) MUST cause the evaluation to stop immediately and report a runtime error. The language does not support `Infinity` or `NaN` as valid values.
- **Ternary Operator Exception**: The ternary operator (`? :`) may produce a valid result even if one of its result branches would produce `NaN`, as long as that branch is not taken. For example, `1 > 0 ? 5 : (0.0/0.0)` is valid and results in `5`.

- Examples:
  - `1 + 2 * 3` → `7` (int).
  - `"hello" + " world"` → `"hello world"`.
  - `1 << 2 ? 4 : 0` → `4`.
  - Errors: `1 = 2` → SyntaxError.

## Non-Functional Specifications

## User Interface
### CLI
- Welcome: "Welcome to Expresso v1.0. Type '!help'; '!quit' to end this session."
- Output: Result on new line; errors prefixed "Error: ".
- Truncation: Outputs >1000 chars: "[truncated...] (full in !n)".

## Testing and Quality Assurance
### Test Cases
- Unit: Each operator (e.g., add_int_float).
- Integration: Nested (e.g., `!(1 && 0) ? 1 : 0` → 1).
- Edge: Divide by zero, max depth (error), invalid literals.
- Negative: `x=1`, `&ptr`, `++i` → errors.

## Clarifications

### Session 2025-10-14
- Q: What is the bit-width for integer values? → A: Machine-dependent signed integer, at least 16-bit.
- Q: What is the fixed buffer size for input mentioned in the security requirements? → A: Use the `1023 bytes` limit already defined in `FR-003` for interactive input.

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
- **Invalid Command-Line Arguments**: Unrecognized flags or malformed arguments MUST cause Expresso to print the error message "expresso: Invalid arguments. -h for help" to stderr and terminate.
- **Resource Cleanup Failure**: System handles potential failures during resource cleanup on exit.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST prompt the user for input with the string "expr>".
- **FR-002**: The system MUST accept ASCII-encoded user input.
- **FR-003**: The system MUST include a line input and editing component that accepts no more than 1023 characters from the TTY terminal.
- **FR-003a**: For file-based or stdin redirected input, lines exceeding 1023 characters MUST be truncated. The system shall report an error for the truncation and then attempt to evaluate the partial (1023-character) input.
- **FR-004**: The system MUST report an error for incomplete expressions.
- **FR-005**: The system MUST report an error for lines ending with a backslash (`\`) followed immediately by a newline character, as this is not a valid escape sequence in Expresso.
- **FR-006**: The system MUST report an error for any extraneous input which follows a valid, complete expression on the same line and is not part of a subsequent valid expression.
- **FR-007**: The system MUST retain a history of the last 10 unique input lines. Uniqueness is determined on a case-sensitive basis. Leading whitespace MUST be preserved, but trailing whitespace MUST be stripped before an entry is added to the history.
- **FR-008**: The system MUST allow users to access input history using up- and down-arrow keys. When displaying the history via `!history` or navigating with arrow keys, each entry MUST be prefixed with a highlighted bracket, its 1-based index, a colon, and a space (e.g., "**[ 1]:** <line text>").
- **FR-009**: The system MUST ignore non-printable ASCII characters (values 0-8, 11-12, 14-31, and 127) in interactive input. Carriage return (13) and linefeed (10) are used to terminate input, and horizontal tab (9) is treated as whitespace.
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

### Non-Functional Requirements

- **NFR-001 (Performance)**: Average evaluation latency MUST be <50ms. This will be benchmarked on a standard GitHub Actions runner using a predefined suite of 1000 representative expressions, none of which exceed 1KB in size.
- **NFR-002 (Performance)**: Peak memory usage MUST be <10MB. This will be measured on the same benchmark environment and suite as NFR-001.
- **NFR-012 (Performance)**: Evaluation of expressions that hit an edge case limit (e.g., max nesting depth or max string length) MUST complete in under 1 second and not result in a hang.
- **NFR-003 (Security)**: The runtime MUST operate in a sandboxed environment. This is achieved by the language grammar itself, which provides no syntax for function calls, file I/O, or any other mechanism that could lead to a system call, ensuring pure computation.
- **NFR-004 (Security)**: Input handling MUST use fixed buffers of 1023 bytes (as per FR-003) to prevent buffer overflows.
- **NFR-005 (Usability)**: Colored output will use standard ANSI escape codes: Green (`\x1b[32m`) for results and Red (`\x1b[31m`) for errors. All color changes MUST be reset with `\x1b[0m`.
- **NFR-006 (Usability)**: The CLI SHOULD attempt to detect if the output stream is a TTY. If it is not a TTY (e.g., output is redirected to a file), ANSI escape codes MUST be disabled automatically.
- **NFR-018 (Accessibility)**: The CLI MUST be compatible with standard screen readers by using standard stdout/stderr streams and avoiding complex terminal cursor manipulation.
- **NFR-007 (Reliability)**: The system MUST always reprompt the user after an error.
- **NFR-008 (Reliability)**: The application MUST not have any memory leaks. This MUST be verified by running the test suite under a memory-checking tool like Valgrind.
- **NFR-019 (Reliability)**: In the event of an unexpected internal error (e.g., memory allocation failure), the system MUST attempt to log the details and print a generic "Internal runtime error occurred" message to the user before terminating with a non-zero exit code.
- **NFR-020 (Security)**: All third-party dependencies MUST be sourced from official, secure channels, and their integrity MUST be verified via checksums (MD5, SHA-256, etc.) or official digital signatures upon download.
- **NFR-021 (Security)**: All internal errors MUST be logged to a local, secure file with stringent permissions (e.g., read-only for the owner, no access for others).
- **NFR-022 (Security)**: The system MUST prevent the output of the most egregious terminal escape sequences (e.g., those that clear the screen, move the cursor arbitrarily, or change terminal modes) when printing user-generated string values to the console. This can be achieved by escaping or filtering known dangerous sequences.
- **NFR-009 (Version Control)**: Generated build artifacts (e.g., from the ANTLR parser generator or the CMake build process) MUST NOT be committed to the version control repository.
- **NFR-010 (Resource Limit)**: The evaluator MUST limit the total number of computational steps (operator evaluations) for a single expression to 1,000,000. If this limit is exceeded, the evaluation MUST halt and report a "Computation too complex" error.
- **NFR-011 (Resource Limit)**: The parser MUST limit the total number of nodes in the Abstract Syntax Tree (AST) for a single expression to 100,000. If this limit is exceeded, parsing MUST halt and report an "Expression too large" error.
- **NFR-013 (Security)**: The input stream MUST be validated. If any byte outside the 7-bit ASCII range (0-127) is detected, the system must report a 'Non-ASCII input detected' error and terminate.
- **NFR-014 (Security)**: Should the language ever be extended with features that interact with external systems (e.g., file I/O), a full security review and input sanitization for that feature will be mandatory.
- **NFR-015 (Security)**: The continuous integration (CI) pipeline MUST include a static analysis security testing (SAST) step.
- **NFR-016 (Security)**: All production builds MUST be compiled with modern security flags enabled, including stack canaries, ASLR, and DEP/NX.
- **NFR-017 (Security)**: A formal threat model is not required for the current version due to the limited, sandboxed nature of the language.

### Key Entities *(include if feature involves data)*

- **Expression**: A user-provided string representing the code to be evaluated.
- **Value**: The result of an evaluation. It can be an integer (machine-dependent signed integer, at least 16-bit), float, character, or string. It is immutable. Floating-point numbers MUST be handled with standard `double` (64-bit) precision.
- **Input History**: A circular buffer storing the last 10 unique input lines.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: The CLI correctly displays the `expr>` prompt and processes user input according to all specified rules (encoding, limits, ignored characters).
- **SC-002**: The input history accurately stores and recalls the last 10 unique input lines via arrow keys.
- **SC-003**: All specified error conditions (input limit, incomplete, extraneous, nesting depth) are detected and reported with appropriate messages.
- **SC-004**: Non-interactive modes (`-e` flag, file input, stdin piping) function correctly, producing expected output and exit codes.
- **SC-005**: The CLI terminates cleanly and silently via all specified exit methods, with all resources deallocated and the correct exit code.
