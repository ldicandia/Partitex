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
	char * string;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
	Statement * statement;
	Statement ** statementList;
	TimeSignature * timeSignature;
	TempoDeclaration * tempoDeclaration;
	NoteSequence * noteSequence;
	Note * note;
	Pattern * pattern;
	Melody * melody;
	RepeatStatement * repeatStatement;
	SimultaneousNotes * simultaneousNotes;
	TimeValue * timeValue;
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
%destructor { destroyStatement($$); } <statement>
%destructor { destroyStatementList($$); } <statementList>
%destructor { destroyTimeSignature($$); } <timeSignature>
%destructor { destroyTempoDeclaration($$); } <tempoDeclaration>
%destructor { destroyNoteSequence($$); } <noteSequence>
%destructor { destroyNote($$); } <note>
%destructor { destroySimultaneousNotes($$); } <simultaneousNotes>
%destructor { destroyTimeValue($$); } <timeValue>
%destructor { destroyPattern($$); } <pattern>
%destructor { destroyMelody($$); } <melody>
%destructor { destroyRepeatStatement($$); } <repeatStatement>
%destructor { free($$); } <string>

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
%token <token> COLON
%token <token> COMMA
%token <token> SEMICOLON_SEP
%token <token> SLASH

// Note duration tokens.
%token <token> WHOLE
%token <token> HALF
%token <token> QUARTER
%token <token> EIGHTH
%token <token> SIXTEENTH

// Tempo tokens.
%token <token> ALLEGRO
%token <token> ANDANTE
%token <token> LARGO
%token <token> PRESTO

// Additional music tokens.
%token <token> NOTES
%token <token> PATTERN
%token <token> MELODY
%token <token> REPEAT
%token <token> SIMULTANEOUS
%token <token> INSTRUMENT
%token <token> TIME_SECONDS
%token <token> TIME_MILLISECONDS
%token <token> TIME_MINUTES
%token <string> IDENTIFIER
%token <note> NOTE_TOKEN


/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program
%type <statement> statement
%type <timeSignature> time_signature
%type <tempoDeclaration> tempo_declaration
%type <noteSequence> note_sequence
%type <note> note_with_duration

%type <program> key_definition
%type <expression> note
%type <token> scale_type
%type <token> note_duration
%type <token> tempo_name

%type <statementList> statement_list
%type <statement> time_statement
%type <statement> tempo_statement
%type <statement> notes_statement
%type <statement> pattern_statement
%type <statement> repeat_statement
%type <statement> melody_statement
%type <statement> simultaneous_statement

%type <simultaneousNotes> simultaneous_definition
%type <timeValue> time_value
%type <token> time_unit

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV
%left COMMA

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression                                      { $$ = ExpressionProgramSemanticAction($1); }
       | statement_list                                 { $$ = StatementListProgramSemanticAction($1); }
       ;

statement_list: statement                               { $$ = SingleStatementListSemanticAction($1); }
              | statement_list statement               { $$ = MultipleStatementListSemanticAction($1, $2); }
              ;

statement: key_definition                              { $$ = KeyDefinitionStatementSemanticAction($1); }
         | time_statement                              { $$ = $1; }
         | tempo_statement                             { $$ = $1; }
         | notes_statement                             { $$ = $1; }
         | pattern_statement                           { $$ = $1; }
         | repeat_statement                            { $$ = $1; }
         | melody_statement                            { $$ = $1; }
         | simultaneous_statement                      { $$ = $1; }
         ;

// Defina la estructura para una key
key_definition: KEY note scale_type SEMICOLON           { $$ = KeyDefinitionSemanticAction($2, $3); }
              ;

// Defina la estructura de una nota con octava (sin punto como separador)
note: NOTE_TOKEN                                       { $$ = NoteFromTokenSemanticAction($1); }
    | DO INTEGER                                        { $$ = NoteOctaveSemanticAction(DO_NOTE, $2); }
    | RE INTEGER                                        { $$ = NoteOctaveSemanticAction(RE_NOTE, $2); }
    | MI INTEGER                                        { $$ = NoteOctaveSemanticAction(MI_NOTE, $2); }
    | FA INTEGER                                        { $$ = NoteOctaveSemanticAction(FA_NOTE, $2); }
    | SOL INTEGER                                       { $$ = NoteOctaveSemanticAction(SOL_NOTE, $2); }
    | LA INTEGER                                        { $$ = NoteOctaveSemanticAction(LA_NOTE, $2); }
    | SI INTEGER                                        { $$ = NoteOctaveSemanticAction(SI_NOTE, $2); }
    ;

// Defina el tipo de escala
scale_type: MAJOR                                       { $$ = $1; }
          | MINOR                                       { $$ = $1; }
          ;

// Time signature statement
time_statement: TIME time_signature SEMICOLON           { $$ = TimeStatementSemanticAction($2); }
              ;

time_signature: INTEGER SLASH INTEGER                   { $$ = TimeSignatureSemanticAction($1, $3); }
              ;

// Tempo statement
tempo_statement: TEMPO tempo_declaration SEMICOLON      { $$ = TempoStatementSemanticAction($2); }
               ;

tempo_name: ALLEGRO                                     { $$ = $1; }
          | ANDANTE                                     { $$ = $1; }
          | LARGO                                       { $$ = $1; }
          | PRESTO                                      { $$ = $1; }
          ;

tempo_declaration: tempo_name                           { $$ = TempoDeclarationSemanticAction($1); }
                 ;

// Notes statement
notes_statement: NOTES COLON note_sequence SEMICOLON    { $$ = NotesStatementSemanticAction($3); }
               ;

note_sequence: note_with_duration                       { $$ = SingleNoteSequenceSemanticAction($1); }
             | note_sequence COMMA note_with_duration   { $$ = MultipleNoteSequenceSemanticAction($1, $3); }
             ;

note_with_duration: note note_duration                  { $$ = NoteWithDurationSemanticAction($1, $2); }
                  ;

note_duration: WHOLE                                    { $$ = WHOLE_NOTE; }
             | HALF                                     { $$ = HALF_NOTE; }
             | QUARTER                                  { $$ = QUARTER_NOTE; }
             | EIGHTH                                   { $$ = EIGHTH_NOTE; }
             | SIXTEENTH                                { $$ = SIXTEENTH_NOTE; }
             ;

// Pattern definition
pattern_statement: PATTERN IDENTIFIER COLON note_sequence SEMICOLON    { $$ = PatternStatementSemanticAction($2, $4); }
                ;

// Melody definition with optional duration
melody_statement: MELODY IDENTIFIER COLON note_sequence SEMICOLON                    { $$ = MelodyStatementSemanticAction($2, $4, NULL); }
                | MELODY IDENTIFIER COLON note_sequence time_value SEMICOLON         { $$ = MelodyStatementSemanticAction($2, $4, $5); }
                ;

// Repeat statement
repeat_statement: REPEAT IDENTIFIER INTEGER SEMICOLON                               { $$ = RepeatStatementSemanticAction($2, $3, NULL); }
                | REPEAT IDENTIFIER INTEGER time_value SEMICOLON                    { $$ = RepeatStatementSemanticAction($2, $3, $4); }
                ;

// Simultaneous notes for multiple instruments
simultaneous_statement: SIMULTANEOUS COLON simultaneous_definition SEMICOLON        { $$ = SimultaneousStatementSemanticAction($3); }
                      ;

simultaneous_definition: INSTRUMENT IDENTIFIER COLON note_sequence                  { $$ = SingleSimultaneousSemanticAction($2, $4); }
                       | simultaneous_definition SEMICOLON_SEP INSTRUMENT IDENTIFIER COLON note_sequence  { $$ = MultipleSimultaneousSemanticAction($1, $4, $6); }
                       ;

// Time value with units
time_value: INTEGER time_unit                          { $$ = TimeValueSemanticAction($1, $2); }
          ;

time_unit: TIME_SECONDS                                { $$ = SECONDS; }
         | TIME_MILLISECONDS                           { $$ = MILLISECONDS; }
         | TIME_MINUTES                                { $$ = MINUTES; }
         ;

// Identifier (handled directly by lexer)

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
