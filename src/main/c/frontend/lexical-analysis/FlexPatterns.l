%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus TokenLexemeAction(TokenLabel label);
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus NoteWithOctaveLexemeAction(NoteType noteType, int octave);

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x IMPORT_EXPRESSION
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

/* "{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); } */

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"def"                            { return TokenLexemeAction(DEF); }
"main"                           { return TokenLexemeAction(MAIN); }
"end"                            { return TokenLexemeAction(END); }
"key"                               { return TokenLexemeAction(KEY); }
/* "tempo"                             { return TokenLexemeAction(TEMPO); } */
"time"                              { return TokenLexemeAction(TIME); }
"major"                             { return TokenLexemeAction(MAJOR); }
"minor"                             { return TokenLexemeAction(MINOR); }
"cleff"                            { return TokenLexemeAction(CLEFF); }
"trebble"                           { return TokenLexemeAction(TREBBLE); }
"bass"                              { return TokenLexemeAction(BASS); }
"trebble_and_bass"                 { return TokenLexemeAction(TREBBLE_AND_BASS); }

"whole"                             { return TokenLexemeAction(WHOLE); }
"half"                              { return TokenLexemeAction(HALF); }
"quarter"                           { return TokenLexemeAction(QUARTER); }
"eighth"                            { return TokenLexemeAction(EIGHTH); }
"sixteenth"                         { return TokenLexemeAction(SIXTEENTH); }
"thirtysecond"                      { return TokenLexemeAction(THIRTYSECOND); }
"sixtyfourth"                       { return TokenLexemeAction(SIXTYFOURTH); }

"allegro"                           { return TokenLexemeAction(ALLEGRO); }
"andante"                           { return TokenLexemeAction(ANDANTE); }
"moderato"                          { return TokenLexemeAction(MODERATO); }
/* "largo"                             { return TokenLexemeAction(LARGO); }
"presto"                            { return TokenLexemeAction(PRESTO); } */

"notes"                             { return TokenLexemeAction(NOTES); }
/* "do"[0-9]+                          { return NoteWithOctaveLexemeAction(DO_NOTE, atoi(yytext + 2)); }
"re"[0-9]+                          { return NoteWithOctaveLexemeAction(RE_NOTE, atoi(yytext + 2)); }
"mi"[0-9]+                          { return NoteWithOctaveLexemeAction(MI_NOTE, atoi(yytext + 2)); }
"fa"[0-9]+                          { return NoteWithOctaveLexemeAction(FA_NOTE, atoi(yytext + 2)); }
"sol"[0-9]+                         { return NoteWithOctaveLexemeAction(SOL_NOTE, atoi(yytext + 3)); }
"la"[0-9]+                          { return NoteWithOctaveLexemeAction(LA_NOTE, atoi(yytext + 2)); }
"si"[0-9]+                          { return NoteWithOctaveLexemeAction(SI_NOTE, atoi(yytext + 2)); } */
"do"|"c"                                { return TokenLexemeAction(DO); }
"re"|"d"                                { return TokenLexemeAction(RE); }
"mi"|"e"                                { return TokenLexemeAction(MI); }
"fa"|"f"                                { return TokenLexemeAction(FA); }
"sol"|"g"                               { return TokenLexemeAction(SOL); }
"la"|"a"                                { return TokenLexemeAction(LA); }
"si"|"b"                                { return TokenLexemeAction(SI); }
"bemol"|"$"                            { return TokenLexemeAction(BEMOL); }
"sharp"|"#"                           { return TokenLexemeAction(SHARP); }
"staccato"                           { return TokenLexemeAction(STACCATO); }
"legato"                            { return TokenLexemeAction(LEGATO); }
"accent"                            { return TokenLexemeAction(ACCENT); }

"set"                              { return TokenLexemeAction(SET); }
"rest"                             { return TokenLexemeAction(REST); }
"pattern"                           { return TokenLexemeAction(PATTERN); }
/* "melody"                            { return TokenLexemeAction(MELODY); } */
"repeat"                            { return TokenLexemeAction(REPEAT); }
/* "simultaneous"                      { return TokenLexemeAction(SIMULTANEOUS); }
"instrument"                        { return TokenLexemeAction(INSTRUMENT); } */
"piano"                            { return TokenLexemeAction(PIANO); }
"mezzo_forte"                      { return TokenLexemeAction(MEZZO_FORTE); }
"forte"                            { return TokenLexemeAction(FORTE); }
"step"                             { return TokenLexemeAction(STEP); }

"seconds"                           { return TokenLexemeAction(TIME_SECONDS); }
"milliseconds"                      { return TokenLexemeAction(TIME_MILLISECONDS); }
"minutes"                           { return TokenLexemeAction(TIME_MINUTES); }

";"                                 { return TokenLexemeAction(SEMICOLON); }
";;"                                { return TokenLexemeAction(SEMICOLON_SEP); }
"."                                 { return TokenLexemeAction(DOT); }
"|"                                 { return TokenLexemeAction(PIPE); }
":"                                 { return TokenLexemeAction(COLON); }
","                                 { return TokenLexemeAction(COMMA); }
"/"                                 { return TokenLexemeAction(SLASH); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }

[[:digit:]]+						{ return IntegerLexemeAction(); }

[a-zA-Z_][a-zA-Z0-9_]*				{ return IdentifierLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
