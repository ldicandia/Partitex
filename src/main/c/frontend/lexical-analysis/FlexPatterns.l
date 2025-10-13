%top{
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
}

%{
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);

/* Nuevas acciones para Partitext */
CompilationStatus KeywordLexemeAction(TokenLabel label);
CompilationStatus IdentifierLexemeAction();
%}

%x MULTILINE_COMMENT

/* === Reusable patterns === */
whitespace      [[:space:]]+
digit           [[:digit:]]+
id              [[:alpha:]_][[:alnum:]_]*
note            (do|re|mi|fa|sol|la|si|[A-Ga-g])
duration        (whole|half|quarter|eighth|sixteenth)
key_kw          "key"
time_kw         "time"
tempo_kw        "tempo"
clef_kw         "clef"
notes_kw        "notes"
rest_kw         "rest"
chord_kw        "chord"
section_kw      "section"
repeat_kw       "repeat"
dynamic_kw      "dynamic"

%%

"/*"                             { return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"          { return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>.             { return IgnoredLexemeAction(); }

/* Palabras clave */
{key_kw}                         { return KeywordLexemeAction(KEY); }
{time_kw}                        { return KeywordLexemeAction(TIME); }
{tempo_kw}                       { return KeywordLexemeAction(TEMPO); }
{clef_kw}                        { return KeywordLexemeAction(CLEF); }
{notes_kw}                       { return KeywordLexemeAction(NOTES); }
{rest_kw}                        { return KeywordLexemeAction(REST); }
{chord_kw}                       { return KeywordLexemeAction(CHORD); }
{section_kw}                     { return KeywordLexemeAction(SECTION); }
{repeat_kw}                      { return KeywordLexemeAction(REPEAT); }
{dynamic_kw}                     { return KeywordLexemeAction(DYNAMIC); }

/* Puntuación y símbolos */
":"                              { return ParenthesisLexemeAction(COLON); }
";"                              { return ParenthesisLexemeAction(SEMICOLON); }
","                              { return ParenthesisLexemeAction(COMMA); }
"{"                              { return ParenthesisLexemeAction(OPEN_BRACE); }
"}"                              { return ParenthesisLexemeAction(CLOSE_BRACE); }
"/"                              { return ParenthesisLexemeAction(SLASH); }

/* Notas y duraciones */
{note}                           { return IdentifierLexemeAction(); }
{duration}                       { return KeywordLexemeAction(DURATION); }

/* Números */
{digit}+                         { return IntegerLexemeAction(); }

/* Espacios en blanco */
{whitespace}                     { return IgnoredLexemeAction(); }

/* Error léxico */
.                                 { return UnknownLexemeAction(); }

/* Fin de archivo */
<<EOF>>                           { return EOFLexemeAction(); }

%%
#include "FlexActions.h"
#include "FlexExport.h"
