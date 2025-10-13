#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions.
 */

Constant *IntegerConstantSemanticAction(const int value);
Expression *ArithmeticExpressionSemanticAction(Expression *leftExpression,
                                               Expression *rightExpression,
                                               ExpressionType type);
Expression *FactorExpressionSemanticAction(Factor *factor);
Factor *ConstantFactorSemanticAction(Constant *constant);
Factor *ExpressionFactorSemanticAction(Expression *expression);
Program *ExpressionProgramSemanticAction(Expression *expression);
Expression *NoteOctaveSemanticAction(NoteType noteType, const int octave);
Program *KeyDefinitionSemanticAction(Expression *note, TokenLabel scaleType);
Program *KeyDefinitionProgramSemanticAction(Program *keyDefinition);

// New semantic actions for extended grammar
Program *StatementListProgramSemanticAction(Statement **statements);
Statement **SingleStatementListSemanticAction(Statement *statement);
Statement **MultipleStatementListSemanticAction(Statement **statements,
                                                Statement *statement);
Statement *KeyDefinitionStatementSemanticAction(Program *keyDefinition);
Statement *TimeStatementSemanticAction(TimeSignature *timeSignature);
Statement *TempoStatementSemanticAction(TempoDeclaration *tempoDeclaration);
Statement *NotesStatementSemanticAction(NoteSequence *noteSequence);
TimeSignature *TimeSignatureSemanticAction(int numerator, int denominator);
TempoDeclaration *TempoDeclarationSemanticAction(TokenLabel tempoName);
NoteSequence *SingleNoteSequenceSemanticAction(Note *note);
NoteSequence *MultipleNoteSequenceSemanticAction(NoteSequence *noteSequence,
                                                 Note *note);
Note *NoteWithDurationSemanticAction(Expression *note, NoteDuration duration);
#endif
