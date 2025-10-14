# Product Requirements Document (PRD): Immutable REPL for Expresso

## 1. Document Information
- **Product Name**: Expresso
- **Version**: 1.0
- **Author**: Product Manager (AI-Assisted Draft)
- **Date**: October 14, 2025
- **Status**: Draft
- **Objective**: Define requirements for a Read-Eval-Print Loop (REPL) tool tailored to Expresso, a minimalist expression-based language with C-style syntax emphasizing immutability, functional purity, and simplicity. This REPL will enable interactive exploration, education, and prototyping in Expresso without supporting stateful mutations or assignments.

## 2. Overview
### 2.1 Product Description
Expresso is an interactive command-line and web-based tool for evaluating expressions in Expresso. Expresso supports basic data types (integers, floats, characters, strings), literals, operators (arithmetic, logical, comparison, bitwise, string manipulation), and nested expressions using C syntax (e.g., precedence, parentheses). Key constraint: No assignment operators (=, +=, etc.) or mutable state—every input is a pure expression that evaluates to an immutable value, which is immediately printed.

This design promotes functional programming principles in a familiar C-like syntax, making it ideal for:
- Educational purposes (teaching C expressions and immutability).
- Quick calculations and data transformations.
- Integration into IDEs or notebooks as a plugin.

The REPL will handle single-line inputs initially, with multi-line expression support (e.g., via parentheses for nesting). Errors will be graceful, providing contextual feedback. No support for pointers, arrays as types (use string literals for sequences if needed), or compound statements.

### 2.2 Background and Problem Statement
Existing REPLs (e.g., Python, JavaScript consoles) rely heavily on stateful variables, and C compilers/interpreters focus on full programs rather than pure expressions. Expresso users need a tool that enforces immutability and expression-only evaluation in C syntax, preventing common pitfalls like side effects or pointer errors. Current alternatives lack this focus, leading to inconsistent experiences. Expresso solves this by providing a pure, assignment-free environment.

### 2.3 Goals and Objectives
- **Primary Goal**: Enable users to evaluate Expresso expressions interactively with immediate, immutable results.
- **Business Objectives**:
  - Increase adoption of Expresso in education and systems programming communities.
  - Achieve 10,000 active users in the first 6 months post-launch.
  - Collect usage analytics to inform Expresso evolution.
- **User Objectives**:
  - Perform calculations without setup overhead.
  - Learn functional concepts through instant feedback in C syntax.
  - Debug expressions in isolation.

### 2.4 Scope
#### In Scope:
- Core REPL loop: Read input, parse/evaluate expression, print result.
- Support for all Expresso data types, literals, operators, and expressions.
- Error handling for syntax, type mismatches, and runtime issues.
- Session history and basic navigation.
- Web and CLI interfaces.
- Extensibility for custom operators (future-proofing).

#### Out of Scope:
- Assignment operators or state persistence across evaluations.
- Pointers, arrays, structs, or any mutable references.
- File I/O or external integrations (e.g., no loading scripts).
- Advanced features like macros or debugging breakpoints (Phase 2).
- Support for mutable data structures or compound statements (e.g., no loops, only ternary conditional expressions).

## 3. User Personas
| Persona | Description | Needs | Pain Points |
|---------|-------------|-------|-------------|
| **Student Learner** | Beginner in C-like languages, using Expresso for expression-focused courses. | Simple interface, clear error messages, examples in REPL. | Confusion from assignments/pointers in full C; needs immutability enforcement. |
| **Developer Prototyper** | Experienced dev testing algorithms or bit manipulations in Expresso. | Fast evaluation, history recall, copy-paste results. | Lack of pure expression evaluators; state leakage in generic REPLs. |
| **Educator** | Teacher creating demos or assignments on C syntax. | Shareable sessions, syntax highlighting. | Tools that allow assignments/pointers, undermining lessons on immutability. |

## 4. Assumptions and Dependencies
- **Assumptions**:
  - Users have basic knowledge of C expressions (e.g., precedence, operators).
  - Expresso parser/evaluator library is available (open-source or internal).
  - 90% of usages will be single-expression evaluations.
- **Dependencies**:
  - Expresso core engine (v1.0) for parsing and evaluation.
  - Runtime environments: Node.js for web, Python/Rust for CLI.
  - No external APIs required initially.

## 5. Requirements
### 5.1 Functional Requirements
| ID | Requirement | Description | Priority |
|----|-------------|-------------|----------|
| FR-01 | Expression Input | Accept single or multi-line strings as pure C-style expressions (e.g., `1 + 2 * 3` or `(a ? b : c)`). Reject assignment syntax (e.g., `x = 5`, `++i`). | High |
| FR-02 | Evaluation Engine | Evaluate using Expresso rules: immutable values only. Operators return new values. Support nesting, C precedence (e.g., * before +). | High |
| FR-03 | Output Printing | Pretty-print results (e.g., formatted for strings/chars). Handle all types: int, float, char, string. | High |
| FR-04 | Error Handling | Parse errors (syntax), type errors (e.g., add string+int), runtime (e.g., divide by zero). Show line/column, suggestion. | High |
| FR-05 | History Management | Store last 100 expressions/results. Navigate with up/down arrows. Command: `!history` to list, `!recall n` to re-eval. | Medium |
| FR-06 | Help System | Command: `!help` shows operators, types, examples. Auto-suggest on tab (CLI). | Medium |
| FR-07 | Session Reset | Command: `!clear` resets history without exiting. | Low |
| FR-08 | Multi-Platform | CLI (terminal) and Web (browser console with syntax highlight via CodeMirror). | High |
| FR-09 | Immutability Enforcement | Reject any mutation attempts (e.g., no =, ++, --). All ops functional (e.g., + on strings concatenates new string). No pointers (&, *). | High |

### 5.2 Non-Functional Requirements
| ID | Category | Requirement | Details | Priority |
|----|----------|-------------|---------|----------|
| NFR-01 | Performance | Evaluation Latency | < 50ms for expressions < 1KB. | High |
| NFR-02 | Security | Input Sanitization | No code injection; treat all input as data. Sandbox evaluation. | High |
| NFR-03 | Usability | Interface | Intuitive prompts (e.g., `expr> `). Accessible (WCAG 2.1). | High |
| NFR-04 | Reliability | Error Recovery | Resume after errors; no crashes. | High |
| NFR-05 | Scalability | Expression Complexity | Handle nested depth up to 100; strings up to 10,000 chars. | Medium |
| NFR-06 | Portability | Platforms | Windows, macOS, Linux (CLI); Chrome/Firefox/Safari (Web). | High |
| NFR-07 | Analytics | Telemetry | Optional opt-in tracking of commands/errors for improvements. | Low |
| NFR-08 | Extensibility | Plugins | API for adding custom types/operators post-launch. | Medium |

### 5.3 Expresso Specifics Supported
- **Data Types**: Integer (int, long), Float (float, double), Character (char), String (immutable char sequences).
- **Literals**: `42`, `3.14`, `'a'`, `"hello"`.
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`, `%`.
  - Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`.
  - Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`.
  - Logical: `&&`, `||`, `!`.
  - Ternary: `cond ? trueExpr : falseExpr`.
  - String: `+` for concat (returns new string).
- **Expressions**: Nested (e.g., `1 + (2 * 3)`), with C precedence and associativity. No function calls beyond built-ins; no arrays/pointers.

## 6. User Stories
- As a student, I want to input `1 + 2 * 3` and see `7` so I can verify C precedence.
- As a developer, I want to nest expressions like `(x > 0 ? x : -x) * 2` and get the result to prototype.
- As an educator, I want errors like "Syntax error at line 1: Assignment operator '=' not allowed" to teach restrictions.

## 7. UI/UX Considerations
- **CLI**: Prompt `expr> `, colored output (green results, red errors).
- **Web**: Input box, output pane, history sidebar. Syntax highlighting for operators/types per C conventions.
- **Flows**:
  1. User enters expression → Parse/Eval → Print or Error.
  2. Use `!commands` for meta-ops.
- **Edge Cases**: Empty input (reprompt), very long outputs (truncate with ellipsis, expandable). Reject pointer syntax (e.g., `&var` errors).

## 8. Technical Considerations
- **Architecture**: Frontend (REPL loop) + Backend (Expresso evaluator WASM for web).
- **Data Flow**: Input → Lexer/Parser (C-subset) → AST → Evaluator → Immutable Value → Serializer → Output.
- **Testing**: Unit (each operator), Integration (nested expr), Edge (bitwise on floats error, max depth).

## 9. Risks and Mitigations
| Risk | Impact | Mitigation |
|------|--------|------------|
| User frustration from no assignments/state | High churn | Provide docs/examples showing composition (e.g., reuse via copy-paste). |
| Parser ambiguity in C expressions | Bugs | Use established parser generator (e.g., yacc/bison subset). |
| Performance on bitwise/large strings | Slowdown | Optimize with native ops in Phase 2. |

## 10. Success Metrics
- Usage: 80% sessions >5 evaluations.
- Retention: 50% return within 7 days.
- NPS: >8/10 from beta users.
- Bug Rate: <1% crashes reported.

## 11. Appendix
- **Expresso Examples**:
  - Input: `1 + 2 * 3` → Output: `7`
  - Input: `"hello" + " world"` → Output: `"hello world"`
  - Input: `'a' < 'b' ? 1 : 0` → Output: `1`
  - Error: `x = 5` → "SyntaxError: Assignments not supported"
  - Error: `&var` → "SyntaxError: Pointers not supported"
- **Future Phases**: Add built-in functions (e.g., strlen), export results, collaborative sessions. 

This PRD serves as a blueprint; iterate based on stakeholder feedback.
