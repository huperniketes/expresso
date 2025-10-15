grammar Expresso;

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
CharacterLiteral: '\'' (~['\\] | '\\.')* '\'';
StringLiteral: '"' (~["\\] | '\\'.)* '"';

WS: [ \t\n\r]+ -> skip; // Custom: Error on assignment ops if needed, but grammar excludes =
