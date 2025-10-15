# Checklist: Core Logic & Language Specification (QA Gate)

**Purpose**: A comprehensive, rigorous checklist to validate the quality and completeness of requirements for the core expression evaluation logic and language specification before formal QA.
**Created**: 2025-10-14
**Feature**: [spec.md](../spec.md)

## Requirement Completeness (Grammar & Operators)

- [ ] CHK001 - Are all operators (arithmetic, logical, bitwise, comparison, conditional) explicitly defined in the grammar with their correct symbols? [Completeness, Spec §Grammar]
- [ ] CHK002 - Is the precedence and associativity of every operator explicitly defined and unambiguous? [Clarity, Spec §Grammar, §Evaluation Rules]
- [ ] CHK003 - Are the rules for handling unary operators (`!`, `~`, `+`, `-`) clearly specified for all applicable types? [Completeness, Spec §Grammar]
- [ ] CHK004 - Does the spec define the behavior for all possible combinations of operators and data types? [Completeness, Spec §FR-017]

## Requirement Clarity (Type System & Coercion)

- [ ] CHK005 - Is the exact bit-width for all integer types explicitly defined, beyond the minimum 16-bit, for a given target architecture? [Clarity, Spec §Key Entities]
- [ ] CHK006 - Are the precise rules for type promotion (e.g., `int` to `float` in mixed arithmetic) clearly documented for all scenarios? [Clarity, Spec §FR-017]
- [ ] CHK007 - Is the behavior for bitwise operations on `char` (treating `char` as `int`) explicitly defined, including potential sign extension? [Clarity, Spec §FR-017]
- [ ] CHK008 - Are the rules for string concatenation with other types (e.g., `string + int`, `int + string`) clearly defined, including the conversion process? [Clarity, Spec §FR-017]

## Edge Case Coverage (Language)

- [ ] CHK009 - Does the spec define the behavior for operations involving floating-point special values (e.g., `NaN`, `Infinity`, `-Infinity`)? [Gap]
- [ ] CHK010 - Are the rules for handling empty strings or null characters in operations clearly specified? [Gap]
- [ ] CHK011 - Is the maximum nesting depth for parenthesized expressions (255) explicitly linked to a defined error message and recovery behavior? [Clarity, Spec §FR-011, §Edge Cases]
- [ ] CHK012 - Does the spec define how string literals must handle all standard C-style escape sequences (e.g., `\n`, `\t`, `\"`, `\xHH`, `\uHHHH`)? [Gap]
- [ ] CHK013 - Is the character set (e.g., ASCII, UTF-8) for string and character types explicitly defined for the language? [Gap]

## Immutability & Side Effects

- [ ] CHK014 - Are the requirements for immutability of `Value`s (all operations produce new `Value`s, no references) clearly stated and consistently applied across all operations? [Clarity, Spec §FR-016]
- [ ] CHK015 - Does the spec explicitly state that the language has no side effects (e.g., no assignment, no pointers, no system calls) and are these constraints enforced? [Completeness, Spec §FR-013, §NFR-003]
