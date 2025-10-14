# Data Model

This document defines the key data entities for the Expresso evaluator, based on the feature specification.

## 1. Expression

- **Description**: Represents the input provided by the user for evaluation.
- **Type**: `String` (or `char*` in C)
- **Constraints**: Immutable.

## 2. Value

- **Description**: Represents the result of an expression evaluation. This is a variant type that can hold one of several primitive types.
- **Type**: Variant / Discriminated Union
- **Possible Types**:
  - **Integer**: Signed, 64-bit integer (`int64_t`).
  - **Float**: Standard `double` precision floating-point number (64-bit).
  - **Character**: A single ASCII character.
  - **String**: A sequence of characters.
- **Constraints**: Immutable. Once a `Value` is created, its content cannot be changed.
