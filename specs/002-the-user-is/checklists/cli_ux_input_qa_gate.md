# Checklist: CLI User Experience & Input Handling (QA Gate)

**Purpose**: A comprehensive, rigorous checklist to validate the quality and completeness of requirements for the CLI user experience and input handling before formal QA.
**Created**: 2025-10-14
**Feature**: [spec.md](../spec.md)

## Requirement Completeness (Input Handling)

- [ ] CHK018 - Is the exact interactive prompt string (`expr> `) specified, including trailing space, for consistent display? [Clarity, Spec §FR-001]
- [ ] CHK019 - Are the specific ASCII characters that are considered 'non-printable' and should be ignored explicitly listed or defined by a range? [Clarity, Spec §FR-009]
- [ ] CHK020 - Does the spec define the precise behavior for input lines exceeding the 1023-byte limit (e.g., truncate, error and discard, error and keep partial input)? [Clarity, Spec §FR-003, §Edge Cases]
- [ ] CHK021 - Are the precise conditions for an 'incomplete expression' (FR-004) explicitly defined to ensure consistent error reporting? [Clarity, Spec §FR-004]
- [ ] CHK022 - Does the spec define the behavior for lines ending with a backslash (FR-005) when it is not part of a valid escape sequence? [Clarity, Spec §FR-005]
- [ ] CHK023 - Are the specific rules for 'extraneous input after a complete expression' (FR-006) explicitly defined? [Clarity, Spec §FR-006]
- [ ] CHK024 - Does the spec define the behavior for command-line arguments that are not recognized flags or valid file paths? [Completeness, Spec §Edge Cases]

## Requirement Clarity & Consistency (Meta-Commands & History)

- [ ] CHK025 - For each meta-command (`!help`, `!history`, `!n`, `!clear`), is its exact output format and content clearly specified? [Clarity, Spec §Meta-Commands]
- [ ] CHK026 - Is the behavior of `!n` (re-input history[n]) clearly defined for out-of-bounds `n` values (e.g., `!999` when only 10 items exist)? [Clarity, Spec §FR-008]
- [ ] CHK027 - Does the spec define how 'unique input lines' are determined for history retention (e.g., case-sensitive, leading/trailing whitespace ignored)? [Clarity, Spec §FR-007]
- [ ] CHK028 - Are the visual indicators for history navigation (e.g., how the recalled line appears on the prompt) specified? [Gap]

## Scenario Coverage (Non-Interactive Modes)

- [ ] CHK029 - Does the spec define the behavior when both file arguments and stdin piping are provided simultaneously? [Gap, Spec §FR-012, §FR-014]
- [ ] CHK030 - Are the precedence rules for the `-e` flag vs. file arguments vs. stdin piping explicitly defined? [Clarity, Spec §FR-013, §FR-014]
- [ ] CHK031 - Does the spec define the behavior when the `-o` flag specifies an invalid or inaccessible file path for output redirection? [Completeness, Spec §FR-015]

## Acceptance Criteria Quality (Exit Behavior)

- [ ] CHK032 - Are all possible exit codes (beyond 0, 1, 2, 3) explicitly defined for all error conditions, including their specific numeric values? [Completeness, Spec §FR-019]
- [ ] CHK033 - Is the 'graceful cleanup of all allocated resources' (FR-017) defined with measurable criteria or specific resource types to be deallocated? [Clarity, Spec §FR-017]
