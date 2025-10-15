# Data Model

This document defines the key data entities for the Expresso evaluator, based on the feature specification.

## 1. Value

- **Description**: Represents the result of an expression evaluation. This is an immutable variant type that can hold one of several primitive types.
- **Type**: Variant / Discriminated Union
- **Possible Types**:
  - **Integer**: Machine-dependent signed integer, at least 16-bit.
  - **Float**: Standard `double` precision floating-point number (64-bit).
  - **Character**: A single ASCII character.
  - **String**: A sequence of characters.
- **Constraints**: Immutable. Once a `Value` is created, its content cannot be changed.

## 2. Expression

- **Description**: A user-provided string representing the code to be evaluated.
- **Type**: `String` (or `char*` in C)
- **Constraints**: Immutable.

## 3. Input History

- **Description**: Stores a record of recent user inputs for recall and reuse.
- **Type**: Circular Buffer
- **Attributes**:
  - `entries`: Last 10 unique input lines.
- **Access**: Via up/down arrow keys.

## 4. Error Types

- **Description**: Structured representation of different categories of errors during parsing and evaluation.
- **Types**:
  - **SyntaxError**:
    - `line`: Line number where error occurred.
    - `col`: Column number where error occurred.
    - `msg`: Descriptive error message.
  - **TypeError**:
    - `position`: Position (e.g., line/col) where type mismatch occurred.
    - `msg`: Descriptive error message.
  - **RuntimeError**:
    - `msg`: Descriptive error message (e.g., division by zero, numeric overflow).
