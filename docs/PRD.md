What would be an outstanding PRD for a REPL for a programming language which has data types, literals, operators, and expressions but it doesn't include syntax for variable assignments. All values are immutable. Every operation and expression results in a new value which is also immutable.

The lexer inputs lines, either in a REPL or from a file, tokenizes the line, and outputs the result to the parser. The parser creates a tree from the tokens which is then evaluated and processed by the runtime system.

## 1. Product Overview

| Product Name | Expresso |
|---|---|
| Product Vision | To create a purely functional and educational Read-Eval-Print-Loop environment. |
| Target Audience | Compiler/Interpreter engineers, language enthusiasts, computer science students, and functional programming practitioners. |
| Release Goal | Deliver a stable, extensible REPL that supports core language features and the ability to redefine language processing behavior at runtime. |

## 2. Goals & Success Metrics

### 2.1 Business Goals (The "Why")
 * G1: Achieve a 50% adoption rate among target users in academic compiler courses within the first 6 months.
 * G2: Establish a foundation for experimentation in language design and software architecture, and future development tools extensions (e.g., language “packs”, toolchains, etc).

### 2.2 Product Goals (The "What")
 * P1: The REPL must enforce strict immutability and ensure all operations result in a new value.
 * P2: The REPL must expose itself and its components (eg, lexer, parser, symbol tables, etc) as first-class, evaluable values.
 * P3: The user must be able to reconfigure the REPL's execution environment by evaluating expressions that result in a new REPL instance which can be made the active REPL in the current session via another expression (or sub-expression).

### 2.3 Success Metrics (Measurable)
 * M1 (Performance): Average expression evaluation time must be <50\text{ms}.
 * M2 (Stability): Zero unhandled runtime exceptions during the redefinition of the lexer or parser.
 * M3 (Adoption): >100 configurations files (.mreplconfig) created by the community.
 * M4 (Nesting Depth): The NM-REPL must reliably support a nesting depth of at least 10 consecutive REPL evaluations (REPL inside REPL inside REPL...).
 * M5 (Serialization): >99\% success rate for serializing and deserializing the result of any valid expression.

## 3. Core Functional Requirements (FRs)

### 3.1 Base Language Requirements (FR1 - FR5)

| ID | Requirement | Description |
|---|---|---|
| FR1 | Pure Immutability | All values (literals, expression results, components) are immutable. Operations return new instances. |
| FR2 | No Variable Assignment | The language must explicitly exclude syntax for variable assignment. |
| FR3 | Data Types & Literals | Support for basic data types (Integer, Float, Boolean, String, etc.) and their corresponding literals. |
| FR4 | Operators | Support for standard arithmetic, comparison, and logical operators. |
| FR5 | Expressions | All input must be a valid expression that evaluates to a new, immutable value. |

### 3.2 Meta-Programming Requirements (FR6 - FR9)

| ID | Requirement | Description |
|---|---|---|
| FR6 | Exposed Components & Accessors | The NM-REPL itself must be exposed as a first-class, named, evaluable operand. All internal components (lexer, parser, symbol tables, interpreter, memory manager, I/O, etc.) must be accessible through accessor operators on the REPL object (e.g., mmC.parser). |
| FR7 | Component Construction | Operations on exposed components must result in a new immutable instance of that component type. This includes using existing components and configuration data to create a derived version (e.g., new_lexer = mmC.lexer + new_rule). |
| FR8 | Runtime Configuration | All components of the NM-REPL, and the REPL itself, are accessible as operands for expressions. New configurations are derived by operating on existing components, producing new instances of the components which can be incorporated into a REPL which can service sessions. |
| FR9 | Nested REPL Evaluation | The NM-REPL must accept another immutable REPL instance as an operand to be evaluated. This evaluation must suspend the current REPL's loop and launch the new REPL, creating a nested session that evaluates expressions based on the new REPL's configuration. |

### 3.3 Persistence Requirements (FR10 - FR11)

| ID | Requirement | Description |
|---|---|---|
| FR10 | Persistence & Serialization | The REPL must be able to serialize and persist the result of any expression (including new component instances and nested REPL objects) to a data file or storage object, and be able to deserialize/import them later. |

## 4. Architectural Requirements (ARs)

| ID | Component | Requirement |
|---|---|---|
| AR1 | REPL Instance | A REPL instance is an immutable object containing all its core components. It is the single source of context for an execution session. |
| AR2 | Configuration Isolation (Functional Purity) | Because all data, including the REPL itself, is immutable, any errors or state changes resulting from the evaluation of an expression are isolated to the newly created REPL instance. Errors in a derived/transformed REPL instance do not affect its parent instance. |
| AR3 | Read-Eval-Print Cycle | The loop must be capable of processing two types of evaluation: 1) Standard expression resulting in a value, and 2) REPL-as-Operand evaluation, resulting in the launch of a nested NM-REPL session. |
| AR4 | Lexer | Must accept a line of text (from I/O or file), tokenize it based on its configuration, and output an immutable list of tokens to the Parser. |
| AR5 | Parser | Must accept the list of tokens, build an immutable Abstract Syntax Tree (AST) for valid expressions, and pass it to the Interpreter. |
| AR6 | Interpreter/Runtime | Must be responsible for: 1) Evaluating validated AST to produce a new immutable value. 2) Managing the REPL's system configuration. |
| AR7 | Memory Management | The system must be designed to handle the creation and destruction of potentially deep stacks of immutable REPL instances, favoring efficient, functional data structures to minimize memory overhead. |

## 5. REPL Cycle & User Experience (UX)

### 5.1 Read-Eval-Print Cycle
 * Read: User enters a line of text.
 * Lex: The current active lexer processes the line into an immutable list of tokens.
 * Parse: The current active parser processes the tokens into an immutable AST for valid expressions.
 * Evaluate: The runtime system evaluates the AST:
   * If it's a standard expression, a new immutable value is computed and returned.
   * If it's a configuration expression (e.g., creating a new lexer), the new component value is computed.
   * If it's a system expression (e.g., \text{mmC eval: (mmC + (mmC.parser + new_rule))}), the runtime's configuration is updated.
 * Print: The resulting immutable value or system feedback is printed to the user.
 * Loop: Repeat.

### 5.2 Command Line Interface (CLI) UX
 * The prompt should clearly indicate the active configuration name (e.g., [BaseLang]>, which could change to [CustomLexer1]>).
 * Provide clear, distinct error messages for syntax errors (from the active \text{lexer}/\text{parser}) and runtime/evaluation errors.
