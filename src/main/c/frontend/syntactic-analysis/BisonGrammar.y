%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	TokenLabel token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

/** Terminals. */
%token <integer> INTEGER
%token <token> ADD
%token <token> CLOSE_BRACE
%token <token> CLOSE_COMMENT
%token <token> CLOSE_PARENTHESIS
%token <token> DIV
%token <token> MUL
%token <token> OPEN_BRACE
%token <token> OPEN_COMMENT
%token <token> OPEN_PARENTHESIS
%token <token> SUB

%token <token> IGNORED
%token <token> UNKNOWN

// Music-specific tokens.
%token <token> KEY
%token <token> TEMPO
%token <token> TIME
%token <token> MAJOR
%token <token> MINOR
%token <token> DO
%token <token> RE
%token <token> MI
%token <token> FA
%token <token> SOL
%token <token> LA
%token <token> SI

// Punctuation tokens.
%token <token> SEMICOLON
%token <token> DOT
%token <token> PIPE


/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

%type <program> key_definition
%type <expression> note
%type <token> scale_type

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression                                      { $$ = ExpressionProgramSemanticAction($1); }
       | key_definition                                 { $$ = KeyDefinitionProgramSemanticAction($1); }
       ;

// Defina la estructura para una key
key_definition: KEY note scale_type SEMICOLON           { $$ = KeyDefinitionSemanticAction($2, $3); }
              ;

// Defina la estructura de una nota con octava
note: DO DOT INTEGER                                    { $$ = NoteOctaveSemanticAction(DO_NOTE, $3); }
    | RE DOT INTEGER                                    { $$ = NoteOctaveSemanticAction(RE_NOTE, $3); }
    | MI DOT INTEGER                                    { $$ = NoteOctaveSemanticAction(MI_NOTE, $3); }
    | FA DOT INTEGER                                    { $$ = NoteOctaveSemanticAction(FA_NOTE, $3); }
    | SOL DOT INTEGER                                   { $$ = NoteOctaveSemanticAction(SOL_NOTE, $3); }
    | LA DOT INTEGER                                    { $$ = NoteOctaveSemanticAction(LA_NOTE, $3); }
    | SI DOT INTEGER                                    { $$ = NoteOctaveSemanticAction(SI_NOTE, $3); }
    ;

// Defina el tipo de escala
scale_type: MAJOR                                       { $$ = $1; }
          | MINOR                                       { $$ = $1; }
          ;

// Acá necesitamos mantener las reglas de expresión para que la gramática esté completa
expression: expression ADD expression                    { $$ = ArithmeticExpressionSemanticAction($1, $3, ADDITION); }
          | expression SUB expression                    { $$ = ArithmeticExpressionSemanticAction($1, $3, SUBTRACTION); }
          | expression MUL expression                    { $$ = ArithmeticExpressionSemanticAction($1, $3, MULTIPLICATION); }
          | expression DIV expression                    { $$ = ArithmeticExpressionSemanticAction($1, $3, DIVISION); }
          | factor                                       { $$ = FactorExpressionSemanticAction($1); }
          ;

factor: constant                                        { $$ = ConstantFactorSemanticAction($1); }
      | OPEN_PARENTHESIS expression CLOSE_PARENTHESIS   { $$ = ExpressionFactorSemanticAction($2); }
      ;

constant: INTEGER                                       { $$ = IntegerConstantSemanticAction($1); }
        ;

%%
