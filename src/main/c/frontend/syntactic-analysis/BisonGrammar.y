%{
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

void yyerror(const YYLTYPE * location, const char * message) {
    fprintf(stderr, "Parse error at line %d: %s\n", location->first_line, message);
}
%}

%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
    signed int integer;
    TokenLabel token;
    char * string;

    Program * program;
    KeyDefinition * key_definition;
    TimeSignature * time_signature;
    NoteSequence * note_sequence;
    Note * note;
    Pattern * pattern;
    Statement * statement;
    Statement ** statement_list;
}

%destructor { destroyProgram($$); } <program>
%destructor { destroyKeyDefinition($$); } <key_definition>
%destructor { destroyTimeSignature($$); } <time_signature>
%destructor { destroyNoteSequence($$); } <note_sequence>
%destructor { destroyNote($$); } <note>
%destructor { destroyPattern($$); } <pattern>
%destructor { destroyStatement($$); } <statement>
%destructor { free($$); } <statement_list>
%destructor { free($$); } <string>

%token <integer> INTEGER
%token <token> DEF KEY TIME TREBBLE BASS TREBBLE_AND_BASS SEMICOLON SLASH NOTES MAIN END PATTERN BEMOL SHARP WHOLE HALF QUARTER EIGHTH SIXTEENTH THIRTYSECOND SIXTYFOURTH STACCATO LEGATO ACCENT SET FORTE PIANO MEZZO_FORTE ALLEGRO ANDANTE MODERATO STEP REST REPEAT COLON MAJOR MINOR
%token <token> DO RE MI FA SOL LA SI
%token <string> IDENTIFIER
%token <token> UNKNOWN

%type <program> program definitions_section notes
%type <statement_list> definitions functions lines
%type <key_definition> key
%type <time_signature> time time_signature
%type <note> note_macro note
%type <pattern> main macro
%type <statement> statement
%type <token> scale_type bemol_sharp figure articulation configurations tempo_configuration dynamics_configuration rest_step
%type <integer> quantifier

%%

program: definitions_section notes { $$ = ProgramSemanticAction($1, $2); }
       ;

definitions_section: DEF COLON definitions { $$ = DefinitionsSectionSemanticAction($3); }
                   | %empty { $$ = NULL; }
                   ;

definitions: key definitions { $$ = AddDefinitionSemanticAction($1, $2); }
           | time definitions { $$ = AddDefinitionSemanticAction($1, $2); }
           | TREBBLE SEMICOLON definitions { $$ = AddDefinitionSemanticAction(ClefStatementSemanticAction(TREBBLE), $3); }
           | BASS SEMICOLON definitions { $$ = AddDefinitionSemanticAction(ClefStatementSemanticAction(BASS), $3); }
           | TREBBLE_AND_BASS SEMICOLON definitions { $$ = AddDefinitionSemanticAction(ClefStatementSemanticAction(TREBBLE_AND_BASS), $3); }
           | %empty { $$ = NULL; }
           ;

key: KEY note_macro scale_type SEMICOLON { $$ = KeyDefinitionSemanticAction($2, $3); }
   ;

time: TIME time_signature SEMICOLON { $$ = TimeStatementSemanticAction($2); }
    ;

time_signature: INTEGER SLASH INTEGER { $$ = TimeSignatureSemanticAction($1, $3); }
              ;

note_macro: note INTEGER { $$ = NoteOctaveSemanticAction($1, $2); }
          | note { $$ = NoteOctaveSemanticAction($1, 4); }
          ;

note: DO { $$ = NoteSemanticAction(DO_NOTE); }
    | RE { $$ = NoteSemanticAction(RE_NOTE); }
    | MI { $$ = NoteSemanticAction(MI_NOTE); }
    | FA { $$ = NoteSemanticAction(FA_NOTE); }
    | SOL { $$ = NoteSemanticAction(SOL_NOTE); }
    | LA { $$ = NoteSemanticAction(LA_NOTE); }
    | SI { $$ = NoteSemanticAction(SI_NOTE); }
    ;

scale_type: MAJOR { $$ = MAJOR; }
          | MINOR { $$ = MINOR; }
          | %empty { $$ = MAJOR; }
          ;

notes: NOTES COLON functions { $$ = NotesStatementSemanticAction($3); }
     ;

functions: main functions { $$ = AddFunctionSemanticAction($1, $2); }
         | macro functions { $$ = AddFunctionSemanticAction($1, $2); }
         | %empty { $$ = NULL; }
         ;

main: MAIN COLON lines END SEMICOLON { $$ = MainFunctionSemanticAction($3); }
    ;

macro: PATTERN IDENTIFIER COLON lines END SEMICOLON { $$ = PatternFunctionSemanticAction($2, $4); }
     ;

lines: note_macro bemol_sharp figure articulation SEMICOLON lines { $$ = SingleLineSemanticAction($1, $2, $3, $4, $6); }
     | SET configurations SEMICOLON lines { $$ = SetConfigurationSemanticAction($2, $4); }
     | rest_step INTEGER SEMICOLON lines { $$ = RestStepSemanticAction($1, $2, $4); }
     | IDENTIFIER quantifier SEMICOLON lines { $$ = FunctionCallSemanticAction($1, $2, $4); }
     | %empty { $$ = NULL; }
     ;

bemol_sharp: BEMOL { $$ = BEMOL; }
           | SHARP { $$ = SHARP; }
           | %empty { $$ = 0; }
           ;

figure: WHOLE { $$ = WHOLE; }
      | HALF { $$ = HALF; }
      | QUARTER { $$ = QUARTER; }
      | EIGHTH { $$ = EIGHTH; }
      | SIXTEENTH { $$ = SIXTEENTH; }
      | THIRTYSECOND { $$ = THIRTYSECOND; }
      | SIXTYFOURTH { $$ = SIXTYFOURTH; }
      | %empty { $$ = QUARTER; }
      ;

articulation: STACCATO { $$ = STACCATO; }
            | LEGATO { $$ = LEGATO; }
            | ACCENT { $$ = ACCENT; }
            | %empty { $$ = 0; }
            ;

quantifier: REPEAT INTEGER { $$ = $2; }
          | %empty { $$ = 1; }
          ;

rest_step: REST { $$ = REST; }
         | STEP { $$ = STEP; }
         ;

configurations: tempo_configuration { $$ = $1; }
             | dynamics_configuration { $$ = $1; }
             ;

tempo_configuration: ALLEGRO { $$ = ALLEGRO; }
                   | ANDANTE { $$ = ANDANTE; }
                   | MODERATO { $$ = MODERATO; }
                   ;

dynamics_configuration: PIANO { $$ = PIANO; }
                      | MEZZO_FORTE { $$ = MEZZO_FORTE; }
                      | FORTE { $$ = FORTE; }
                      ;

%%