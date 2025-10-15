# Checklist: Non-Functional Requirements (QA Gate)

**Purpose**: A comprehensive, rigorous checklist to validate the quality and completeness of non-functional requirements (Performance, Security, Usability, Reliability) before formal QA.
**Created**: 2025-10-14
**Feature**: [spec.md](../spec.md)

## Performance Requirements Quality

- [ ] CHK093 - Are the specific conditions (e.g., hardware, OS, expression complexity, input size) under which the `<50ms` latency benchmark is measured explicitly defined? [Clarity, Spec §NFR-001]
- [ ] CHK094 - Are the specific conditions (e.g., hardware, OS, expression complexity, input size) under which the `<10MB` peak memory usage benchmark is measured explicitly defined? [Clarity, Spec §NFR-002]
- [ ] CHK095 - Does the spec define acceptable performance degradation under extreme edge cases (e.g., maximum nesting depth, maximum string length)? [Gap]

## Security Requirements Quality

- [ ] CHK096 - Is the exact nature of the 'sandboxed environment' (NFR-003) explicitly defined, including which system calls are disallowed or restricted? [Clarity, Spec §NFR-003]
- [ ] CHK097 - Are there explicit requirements to validate and sanitize all user input to prevent potential injection attacks, should the language ever be extended with system calls? [Gap]
- [ ] CHK098 - Are there requirements defining how to handle malformed or malicious byte sequences in user input, especially if supporting multi-byte character sets like UTF-8? [Gap]
- [ ] CHK099 - Does the spec define strict limits on computational resources (CPU time, memory allocation) for a single evaluation to mitigate DoS attacks from 'billion laughs' style or deeply recursive expressions? [Completeness, Spec §NFR-001, §NFR-002]
- [ ] CHK100 - Is there a requirement to use a static analysis security testing (SAST) tool as a mandatory step in the CI/build process? [Gap]
- [ ] CHK101 - Are modern compiler security flags (e.g., `-fstack-protector-strong`, ASLR, DEP/NX) required to be enabled for all production builds? [Gap]
- [ ] CHK102 - Is a formal threat model for the Expresso application required to be documented, outlining potential attackers, attack vectors, and mitigations? [Gap]

## Usability Requirements Quality

- [ ] CHK103 - Are the specific ANSI escape codes for colored output (green for results, red for errors) explicitly defined? [Clarity, Spec §NFR-005]
- [ ] CHK104 - Does the spec define how the CLI detects terminal support for ANSI escape codes and switches to plain text fallback? [Clarity, Spec §NFR-006]
- [ ] CHK105 - Are there requirements for accessibility beyond plain text fallback (e.g., screen reader compatibility)? [Gap]

## Reliability Requirements Quality

- [ ] CHK106 - Is the 'graceful cleanup of all allocated resources' (NFR-008) defined with measurable criteria or specific resource types to be deallocated? [Clarity, Spec §NFR-008]
- [ ] CHK107 - Does the spec define the expected behavior for unexpected internal errors (e.g., memory allocation failure) beyond just 'always reprompt'? [Gap]
