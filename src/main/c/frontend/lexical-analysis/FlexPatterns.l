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

"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"key"                               { return TokenLexemeAction(KEY); }
"tempo"                             { return TokenLexemeAction(TEMPO); }
"time"                              { return TokenLexemeAction(TIME); }
"major"                             { return TokenLexemeAction(MAJOR); }
"minor"                             { return TokenLexemeAction(MINOR); }

"whole"                             { return TokenLexemeAction(WHOLE); }
"half"                              { return TokenLexemeAction(HALF); }
"quarter"                           { return TokenLexemeAction(QUARTER); }
"eighth"                            { return TokenLexemeAction(EIGHTH); }
"sixteenth"                         { return TokenLexemeAction(SIXTEENTH); }

"allegro"                           { return TokenLexemeAction(ALLEGRO); }
"andante"                           { return TokenLexemeAction(ANDANTE); }
"largo"                             { return TokenLexemeAction(LARGO); }
"presto"                            { return TokenLexemeAction(PRESTO); }

"do"                                { return TokenLexemeAction(DO); }
"re"                                { return TokenLexemeAction(RE); }
"mi"                                { return TokenLexemeAction(MI); }
"fa"                                { return TokenLexemeAction(FA); }
"sol"                               { return TokenLexemeAction(SOL); }
"la"                                { return TokenLexemeAction(LA); }
"si"                                { return TokenLexemeAction(SI); }
"notes"                             { return TokenLexemeAction(NOTES); }

";"                                 { return TokenLexemeAction(SEMICOLON); }
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

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
