# expresso Development Guidelines

Auto-generated from all feature plans. Last updated: 2025-10-14
Test-first Development Processes (TDD)
- Write tests before implementation.
- Each feature or user story must be independently testable.
- Use unit tests for core logic and integration tests for end-to-end scenarios.
- Do not run complete test suite. Only run tests related to current task.

## Active Technologies
- ANSI C17 (Core application), C++17 (for ANTLR-generated parser) + ANTLR4 (Parser Generator) (001-expresso-is-an)
- N/A (In-memory evaluation) (002-the-user-is)

## Project Structure
```
src/
tests/
```

## Commands
# Add commands for ANSI C17 (Core application), C++17 (for ANTLR-generated parser)

## Code Style
ANSI C17 (Core application), C++17 (for ANTLR-generated parser): Follow standard conventions

## Testing
- Unit Tests: GoogleTest for C++, Custom assert.h for C
- Reporting failure: Include the received result (output, values, etc) from the test subject to the assert message text for diagnosis.
- Integration Tests: Custom scripts in tests/integration/
- Do not run complete test suite. Only run tests related to current task.

## Recent Changes
- 002-the-user-is: Added ANSI C17 (Core application), C++17 (for ANTLR-generated parser) + ANTLR4 (Parser Generator)
- 001-expresso-is-an: Added ANSI C17 (Core application), C++17 (for ANTLR-generated parser) + ANTLR4 (Parser Generator)

<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
