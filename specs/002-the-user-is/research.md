# Research & Decisions

This document records the decisions made to resolve the unknowns identified in the implementation plan.

## 1. C/C++ Testing Framework

- **Topic**: Selection of a testing framework for a mixed C/C++ project managed by CMake.
- **Decision**: Use GoogleTest for C++ code (parser) and a simple, custom `assert.h`-based framework for the C code (core library). Both test suites will be managed and executed by CTest.
- **Rationale**: GoogleTest is a mature, feature-rich, and widely-used framework for C++. For the core C17 library, a full framework is not necessary; a lightweight, header-only assertion system is sufficient and avoids adding another external dependency. CTest is the standard test runner for CMake projects and is well-suited to manage multiple test suites.
- **Alternatives Considered**:
  - Using GoogleTest for the C code (possible, but can be syntactically awkward).
  - Using a dedicated C testing framework like Check (this would add another dependency to manage).

## 2. C++/C17 Integration Strategy

- **Topic**: How to integrate the C++-based ANTLR parser with the main C17 application.
- **Decision**: The C++ parser will be built as a static library. A thin C-compatible wrapper will be created for the parser class, exposing its functionality using `extern "C"`. The main C application will link against this static library and call the C wrapper functions.
- **Rationale**: This is a standard, robust pattern for integrating C++ code into a C application. It provides a clean separation of concerns, hides the C++ implementation details from the C code, and prevents issues like C++ name mangling from breaking the C linker.
- **Alternatives Considered**:
  - Inter-process communication (too complex for this use case).
  - Rewriting the ANTLR C++ target to produce C code (not feasible).
