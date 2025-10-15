# CLI Interface Contract

This document defines the public contract for the Expresso command-line interface, detailing its invocation, input/output behavior, and available commands.

## Invocation

The `expresso` executable can be invoked in several ways:

1.  **Interactive Mode (Default)**:
    ```sh
    ./expresso
    ```
    Starts an interactive session with the `expr> ` prompt.

2.  **File Input Mode**:
    ```sh
    ./expresso <file_path>
    ```
    Evaluates expressions from the specified file. Each line is treated as an expression.

3.  **Non-Interactive (Execute) Mode**:
    ```sh
    ./expresso -e "<expression>"
    ```
    Evaluates a single expression provided as a string argument. No interactive prompt.

4.  **Stdin Pipe Mode**:
    ```sh
    echo "<expression>" | ./expresso
    # or
    cat <file_path> | ./expresso
    ```
    Evaluates expressions piped from standard input. No interactive prompt.

## Command-Line Arguments

-   `-e "<expression>"`: Execute or non-interactive mode. Evaluates the provided expression string and exits. (FR-013)
-   `-o <output_file>`: Redirects standard output to the specified file. Can be used with file input or non-interactive mode. (FR-015)

## Interactive Mode Behavior

-   **Prompt**: The user is prompted with `expr> ` (FR-001).
-   **Input Handling**:
    -   Accepts ASCII-encoded input (FR-002).
    -   Input lines are limited to 1023 bytes (FR-003).
    -   Non-printable characters (excluding CR/LF) are ignored (FR-009).
    -   Empty lines or lines with only whitespace/ignored non-printable characters are ignored, and the prompt is re-displayed (FR-010).
-   **History**: Retains the last 10 input lines, accessible via up/down arrow keys (FR-007, FR-008).

## Meta-Commands (Interactive Mode Only)

Meta-commands start with `!` and are dispatched internally (FR-003).

| Command | Behavior |
|---------|----------|
| `!help` | Prints a table of command-line arguments, operators, types, and examples (e.g., "1+2*3 → 7"). |
| `!history` | Lists the last 10 input lines (FR-007). |
| `!n`    | Re-inputs the expression from history at index `n` (FR-008). |
| `!clear` | Clears the input history and prints "Session history cleared." (FR-009). |

## Output

-   **Results**: Valid expression results are printed to stdout on a new line (FR-020).
-   **Errors**: Errors are prefixed with "Error: " and printed to stderr (FR-020).
-   **Truncation**: Outputs exceeding 1000 characters are truncated, indicating `[truncated...] (full in !n)` (FR-021).

## Exit Behavior

-   **Commands**: Session terminates upon `!exit`, `!quit`, or Ctrl-D (FR-016).
-   **Cleanup**: All allocated resources are cleaned up upon termination (FR-017).
-   **Silence**: Terminates silently (without printing messages) upon a clean exit (FR-018).
-   **Exit Codes**: Terminates with an exit code reflecting the status of the last command executed (FR-019):
    -   `0`: Success
    -   `1`: Syntax Error
    -   `2`: Evaluation Error
    -   `3`: Division-by-zero
    -   *(and so on for other specific error types)*
