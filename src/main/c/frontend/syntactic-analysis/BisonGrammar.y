%{
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

void yyerror(const YYLTYPE * location, const char * message) {}
%}

/* --- Configuración Bison --- */
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

/* --- Valores semánticos --- */
%union {
	signed int integer;
	TokenLabel token;

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
}

/* --- Destructores --- */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

/* --- Tokens del lenguaje --- */
%token <integer> INTEGER
%token <token> ID
%token <token> KEY TIME TEMPO CLEF NOTES REST CHORD SECTION REPEAT DYNAMIC
%token <token> DURATION
%token <token> OPEN_BRACE CLOSE_BRACE COMMA SEMICOLON COLON SLASH
%token <token> IGNORED UNKNOWN

/* --- No-terminales --- */
%type <program> program
%type <expression> header sections section body notes_block note_list note

%%

/* === GRAMÁTICA DE PARTITEXT === */

program:
	header sections { $$ = ExpressionProgramSemanticAction(NULL); }
	;

header:
	key_decl time_decl tempo_decl opt_clef
	;

key_decl:
	KEY ID ID SEMICOLON
	;

time_decl:
	TIME INTEGER SLASH INTEGER SEMICOLON
	;

tempo_decl:
	TEMPO ID SEMICOLON
	;

opt_clef:
	| CLEF ID SEMICOLON
	;

sections:
	sections section
	| section
	;

section:
	SECTION ID OPEN_BRACE body CLOSE_BRACE
	;

body:
	notes_block
	| chord_block
	| rest_block
	| repeat_block
	;

notes_block:
	NOTES COLON note_list SEMICOLON
	;

note_list:
	note
	| note_list COMMA note
	;

note:
	ID INTEGER DURATION
	;

chord_block:
	CHORD OPEN_BRACE note_list CLOSE_BRACE DURATION SEMICOLON
	;

rest_block:
	REST DURATION SEMICOLON
	;

repeat_block:
	REPEAT INTEGER OPEN_BRACE body CLOSE_BRACE
	;

%%

