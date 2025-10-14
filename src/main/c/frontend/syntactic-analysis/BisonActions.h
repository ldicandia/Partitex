#ifndef BISON_ACTIONS_HEADER
#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

ModuleDestructor initializeBisonActionsModule(CompilerState *compilerState);

Program *ProgramSemanticAction(Program *definitions_section, Program *notes);
Program *DefinitionsSectionSemanticAction(Statement **definitions);
Statement **AddDefinitionSemanticAction(Statement *definition, Statement **definitions);
KeyDefinition *KeyDefinitionSemanticAction(Note *note, TokenLabel scale_type);
Statement *TimeStatementSemanticAction(TimeSignature *time_signature);
Statement *ClefStatementSemanticAction(TokenLabel clef);
TimeSignature *TimeSignatureSemanticAction(int numerator, int denominator);
Program *NotesStatementSemanticAction(Statement **functions);
Statement **AddFunctionSemanticAction(Pattern *function, Statement **functions);
Pattern *MainFunctionSemanticAction(Statement **lines);
Pattern *PatternFunctionSemanticAction(char *name, Statement **lines);
Statement **SingleLineSemanticAction(Note *note, TokenLabel bemol_sharp, TokenLabel figure, TokenLabel articulation, Statement **lines);
Statement **SetConfigurationSemanticAction(TokenLabel configuration, Statement **lines);
Statement **RestStepSemanticAction(TokenLabel type, int number, Statement **lines);
Statement **FunctionCallSemanticAction(char *identifier, int quantifier, Statement **lines);
Note *NoteOctaveSemanticAction(Note *note, int octave);
Note *NoteSemanticAction(NoteType note_type);

#endif