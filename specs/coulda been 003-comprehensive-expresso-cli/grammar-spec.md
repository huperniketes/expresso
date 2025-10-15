## Expresso Language Specification
### Grammar 
```grammar
Expresso;

expression
    : conditionalExpression
    ;

conditionalExpression
    : logicalOrExpression ('?' expression ':' conditionalExpression)?
    ;

logicalOrExpression
    : logicalAndExpression ('|' '|' logicalAndExpression)*
    ;

logicalAndExpression
    : bitwiseOrExpression ('&&' bitwiseOrExpression)*
    ;

bitwiseOrExpression
    : bitwiseXorExpression ('|' bitwiseOrExpression)*
    ;

bitwiseXorExpression
    : bitwiseAndExpression ('^' bitwiseAndExpression)*
    ;

bitwiseAndExpression
    : equalityExpression ('&' equalityExpression)*
    ;

equalityExpression
    : relationalExpression (('==' | '!=') relationalExpression)*
    ;

relationalExpression
    : shiftExpression (('<' | '>' | '<=' | '>=') shiftExpression)*
    ;

shiftExpression
    : additiveExpression (('<<' | '>>') additiveExpression)*
    ;

additiveExpression
    : multiplicativeExpression (('+' | '-') multiplicativeExpression)*
    ;

multiplicativeExpression
    : unaryExpression (('*' | '/' | '%') unaryExpression)*
    ;

unaryExpression
    : ('!' | '~' | '+' | '-') unaryExpression
    | primaryExpression
    ;
    // Reject assignments/pointers via semantic checks or lexer modes

primaryExpression
    : literal
    | '(' expression ')'
    ;

literal
    : IntegerLiteral
    | FloatingLiteral
    | CharacterLiteral
    | StringLiteral
    ;

// Lexers (C-style)
IntegerLiteral: [0-9]+ | '0x'[0-9a-fA-F]+;
FloatingLiteral: [0-9]* '.' [0-9]+;
CharacterLiteral: '\'' (~['\\] | '\\'.)* '\'';
StringLiteral: '"' (~["\\] | '\\'.)* '"';

WS: [ 	
]+ -> skip; // Custom: Error on assignment ops if needed, but grammar excludes = 
```
- Precedence: Standard C order.
- Extensions: Semantic phase in evaluator rejects invalid ops (e.g., & as pointer). 

### Semantics

- Evaluation: Left-associative; short-circuit for &&/||/?:.
- Immutability: All ops produce new Values; no refs.
- Type Rules:
    - + on strings: concat values to new immutable string.
    - Bitwise on char: ASCII code.
    - No arrays; strings are atomic.
