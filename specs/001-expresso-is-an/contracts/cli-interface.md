# CLI Interface Contract

This document defines the public contract for the Expresso command-line interface.

## Invocation

The tool is invoked by running the `expresso` executable:

```sh
./expresso
```

This will start the interactive session.

## Standard Input (stdin)

- The tool reads expressions line by line from standard input.
- Each line is treated as a separate expression to be evaluated.

## Standard Output (stdout)

- For each valid expression, the tool prints the resulting `Value` to standard output, followed by a newline character.
- Example:
  ```
  > 10 + 5
  15
  > "hello" + " world"
  hello world
  ```

## Standard Error (stderr)

- User-facing errors (e.g., syntax errors, division by zero, numeric overflow) are printed to standard error, followed by a newline character.
- Example:
  ```
  > 10 + * 5
  Error: Invalid syntax
  ```

## Commands

### `exit`

- **Description**: Terminates the interactive session.
- **Usage**:
  ```
  > exit
  ```
