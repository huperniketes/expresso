# SpecKit Specification: Expresso REPL

## 1. Introduction
### 1.1 Purpose
This SpecKit specification outlines the complete design, architecture, functional behaviors, and implementation details for Expresso, an immutable REPL for the Expresso language. Expresso is a minimalist expression-based language with C-style syntax, supporting data types (integers, floats, characters, strings), literals, operators, and expressions, while enforcing immutability and excluding assignments, pointers, or mutable state. Every evaluation produces a new immutable value.

This spec translates the PRD into actionable engineering guidelines, focusing on CLI-first implementation (web version deferred to future stage). It uses ANTLR4 for parsing (generated in C++ runtime).

### 1.2 Scope
- **In Scope**: CLI REPL core, Expresso expression evaluation, error handling, history, help system.
- **Out of Scope**: Web interface, pointers/arrays/structs, state persistence, external integrations, advanced debugging.

### 1.3 Definitions
- **Expression**: A pure Expresso string evaluable to an immutable value (e.g., `1 + 2 * 3`).
- **Immutable Value**: Result of evaluation; cannot be altered post-creation.
- **Meta-Command**: Prefixed with `!` (e.g., `!help`); handled outside evaluation.

### 1.4 References
- PRD for Expresso v1.0.
- ANTLR4 Documentation (https://www.antlr.org/).
- ANSI C17 Standard (ISO/IEC 9899:2018).
- Expresso Grammar (defined in Section 4).

## 2. System Architecture
### 2.1 High-Level Overview
Expresso is structured as a modular CLI application:
- **Frontend**: User input loop (custom readline-like in C for history/navigation).
- **Parser**: ANTLR4-generated lexer/parser (C++ runtime) integrated via C++ bridge or static libs.
- **Evaluator**: AST visitor in C that computes immutable results.
- **Output Handler**: Serializer for results/errors.
- **Runtime**: Sandboxed execution ensuring no side effects.

Tech Stack:
- Core Language: ANSI C17 (for evaluator, input manager, etc.).
- Parser: ANTLR4-generated C++ (linked via shared library or static; C wrappers for integration).
- Dependencies: ANTLR4 C++ runtime, POSIX/Linux libs for CLI (portable via conditional compilation).
- Build: CMake for cross-platform (Windows/MSVC, macOS/clang, Linux/gcc).
- Deployment: Standalone executable (cross-compiled for Windows/macOS/Linux).

Data Flow:
1. Read input → Classify (meta-command or expression).
2. For expressions: Lex → Parse (ANTLR4 C++) → AST (export to C structs) → Evaluate → Serialize → Print.
3. For meta: Execute internally → Print.

### 2.2 Components
| Component | Description | Implementation Notes |
|-----------|-------------|----------------------|
| Input Manager | Handles prompts, history (last 100 entries), arrow navigation. | Custom buffer in C; use termios for Unix, Console API for Windows; store history in memory (struct array). |
| Classifier | Checks if input starts with `!` or is empty. | Simple strncmp check. |
| Parser | ANTLR4 g4 file generates C++ lexer/parser. Rejects assignments/pointers early. | Define grammar in Expresso.g4; generate with `antlr4 -Dlanguage=Cpp`; custom error listener in C++. Wrap AST nodes for C access (e.g., via void* handles). |
| Evaluator | Recursive AST visitor in C; enforces immutability (e.g., + creates new string). | Value union/struct (Int, Float, Char, String); handle type promotion (int→float); dynamic string allocation (strdup). |
| Error Handler | Collects syntax/type/runtime errors; provides position-aware messages. | ANTLR4 DiagnosticErrorListener; custom structs for errors passed back to C. |
| Help/System Manager | Handles `!help`, `!history`, `!recall n`, `!clear`. | In-memory storage; fprintf for tabular output. |
| Output Formatter | Pretty-prints values (e.g., quote strings, hex for chars if needed). | Colored CLI  (ANSI escape codes: green for results, red for errors; fallback to plain). |

### 2.3 Deployment Diagram
- Single executable: `expresso-cli`.
- No server; local execution.
- Future Web: WASM port of evaluator, but deferred.
- Integration: C++ parser lib linked (e.g., libantlr4-runtime.a); C main calls C++ via extern "C" wrappers.

## 3. Data Model
### 3.1 Value Representation
All values are immutable; use a tagged union in C:
```c
typedef enum { VAL_INT, VAL_FLOAT, VAL_CHAR, VAL_STRING } ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t int_val;
        double float_val;
        char char_val;
        char* str_val;  // Owned, immutable; free after use
    } data;
} Value;
```
- Type Coercion Rules:
  - Arithmetic: Promote int to float if mixed.
  - String +: Concatenate with any (sprintf conversion).
  - Bitwise: Only on ints/chars (treat char as int).
- Memory: Caller owns freeing strings via value_free().

### 3.2 History Entry
```c
typedef struct {
    size_t index;
    char* input;
    char* output;  // NULL if error
    char* error;
} HistoryEntry;
```
- Max 100; array-based cyclic buffer.

### 3.3 Error Types
- SyntaxError: Struct with line, col, msg.
- TypeError: Position, msg.
- RuntimeError: Msg only.
- Use char* for messages; enums for categories.

## 4. Expresso Language Specification
### 4.1 Grammar (ANTLR4 Expresso.g4)
```
grammar Expresso;

expression
    :   conditionalExpression
    ;

conditionalExpression
    :   logicalOrExpression ('?' expression ':' conditionalExpression)?
    ;

logicalOrExpression
    :   logicalAndExpression ('||' logicalAndExpression)*
    ;

logicalAndExpression
    :   bitwiseOrExpression ('&&' bitwiseOrExpression)*
    ;

bitwiseOrExpression
    :   bitwiseXorExpression ('|' bitwiseXorExpression)*
    ;

bitwiseXorExpression
    :   bitwiseAndExpression ('^' bitwiseAndExpression)*
    ;

bitwiseAndExpression
    :   equalityExpression ('&' equalityExpression)*
    ;

equalityExpression
    :   relationalExpression (('==' | '!=') relationalExpression)*
    ;

relationalExpression
    :   shiftExpression (('<' | '>' | '<=' | '>=') shiftExpression)*
    ;

shiftExpression
    :   additiveExpression (('<<' | '>>') additiveExpression)*
    ;

additiveExpression
    :   multiplicativeExpression (('+' | '-') multiplicativeExpression)*
    ;

multiplicativeExpression
    :   unaryExpression (('*' | '/' | '%') unaryExpression)*
    ;

unaryExpression
    :   ('!' | '~' | '+'  | '-') unaryExpression
    |   primaryExpression
    ;

// Reject assignments/pointers via semantic checks or lexer modes
primaryExpression
    :   literal
    |   '(' expression ')'
    ;

literal
    :   IntegerLiteral
    |   FloatingLiteral
    |   CharacterLiteral
    |   StringLiteral
    ;

// Lexers (C-style)
IntegerLiteral: [0-9]+ | '0x'[0-9a-fA-F]+;
FloatingLiteral: [0-9]* '.' [0-9]+;
CharacterLiteral: '\'' (~['\\] | '\\' .) '\'';
StringLiteral: '"' (~["\\] | '\\' .)* '"';

WS: [ \t\r\n]+ -> skip;

// Custom: Error on assignment ops if needed, but grammar excludes =
```
- Precedence: Standard C order.
- Extensions: Semantic phase in evaluator rejects invalid ops (e.g., & as pointer). ANTLR4 generate with -visitor for AST walking.
- Parser Generation: `antlr4 -Dlanguage=Cpp Expresso.g4`; integrate via #include <antlr4-runtime.h>.

### 4.2 Semantics
- Evaluation: Left-associative; short-circuit for &&/||/?:.
- Immutability: All ops produce new Values; no refs.
- Type Rules:
  - + on strings: concat (strcat new buffer).
  - Bitwise on char: ASCII code.
  - No arrays; strings are atomic.

## 5. Functional Specifications
### 5.1 Core Loop
- Prompt: `expresso> `.
- On Enter:
  - Empty/WS: Reprompt.
  - !command: Dispatch.
  - Else: Parse → Eval → Print.
- Exit: Ctrl+C/D or `!exit`.

### 5.2 Meta-Commands
| Command | Behavior |
|---------|----------|
| !help | Print table of operators, types, examples (e.g., "1+2*3 → 7"). |
| !history | List last entries: "#1: input → output". |
| !recall n | Re-input history[n]. |
| !clear | Clear history; print "Session cleared." |

### 5.3 Evaluation Rules
- Support nested depth ≤100 (recursion limit with stack check).
- String limit: 10,000 chars (check strlen).
- Examples:
  - `1 + 2 * 3` → `7` (int).
  - `"hello" + " world"` → `"hello world"`.
  - `1 << 2 ? 4 : 0` → `4`.
  - Errors: `1 = 2` → SyntaxError (ANTLR4 fails parse).

## 6. Non-Functional Specifications
### 6.1 Performance
- Latency: <50ms for <1KB input (benchmark on i7 equivalent).
- Memory: <10MB peak; use valgrind for leaks.

### 6.2 Security
- Sandbox: No system calls; pure computation.
- Input: fgets with bounds; no overflows (fixed buffers).

### 6.3 Usability
- Colors: ANSI escapes; detect terminal support.
- Accessibility: Plain text fallback.

### 6.4 Reliability
- Tests: 95% coverage (CMockery or similar for C unit).
- Error Recovery: Always reprompt post-error; graceful cleanup.

## 7. User Interface
### 7.1 CLI
- Welcome: "Welcome to Expresso v1.0. Type !help."
- Output: Result on new line; errors prefixed "Error: ".
- Truncation: Outputs >1000 chars: "[truncated...] (full in !recall)".

## 8. Testing and Quality Assurance
### 8.1 Test Cases
- Unit: Each operator (e.g., add_int_float).
- Integration: Nested (e.g., `!(1 && 0) ? 1 : 0` → 1).
- Edge: Divide by zero, max depth (error), invalid literals.
- Negative: `x=1`, `&ptr`, `++i` → errors.

### 8.2 Tools
- Unity/Check for C tests; ANTLR TestRig for grammar.
- Static Analysis: cppcheck, clang-tidy (for C++ parser).

## 9. Deployment and Maintenance
- Build: CMakeLists.txt; GitHub Actions CI (build/test on platforms).
- Release: Binary downloads (zip with executables).
- Analytics: Opt-in flag (--telemetry); log anonymized errors to file.
- Future: Web stage - port to Emscripten/WASM, integrate CodeMirror.

This SpecKit is complete for CLI implementation; review with engineering.
