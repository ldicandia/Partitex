#include "BisonActions.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static CompilerState *_compilerState = NULL;
static Logger *_logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
  if (_logger != NULL) {
    logDebugging(_logger, "Destroying module: BisonActions...");
    destroyLogger(_logger);
    _logger = NULL;
  }
  _compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState *compilerState) {
  _compilerState = compilerState;
  _logger = createLogger("BisonActions");
  return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char *functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char *functionName) {
  logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant *IntegerConstantSemanticAction(const int value) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Constant *constant = calloc(1, sizeof(Constant));
  constant->value = value;
  return constant;
}

Expression *ArithmeticExpressionSemanticAction(Expression *leftExpression,
                                               Expression *rightExpression,
                                               ExpressionType type) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Expression *expression = calloc(1, sizeof(Expression));
  expression->leftExpression = leftExpression;
  expression->rightExpression = rightExpression;
  expression->type = type;
  return expression;
}

Expression *FactorExpressionSemanticAction(Factor *factor) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Expression *expression = calloc(1, sizeof(Expression));
  expression->factor = factor;
  expression->type = FACTOR;
  return expression;
}

Factor *ConstantFactorSemanticAction(Constant *constant) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Factor *factor = calloc(1, sizeof(Factor));
  factor->constant = constant;
  factor->type = CONSTANT;
  return factor;
}

Factor *ExpressionFactorSemanticAction(Expression *expression) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Factor *factor = calloc(1, sizeof(Factor));
  factor->expression = expression;
  factor->type = EXPRESSION;
  return factor;
}

Program *ExpressionProgramSemanticAction(Expression *expression) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Program *program = calloc(1, sizeof(Program));
  program->expression = expression;
  _compilerState->abstractSyntaxtTree = program;
  return program;
}

Expression *NoteOctaveSemanticAction(NoteType noteType, const int octave) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Expression *expression = calloc(1, sizeof(Expression));
  expression->type = NOTE;
  expression->note = calloc(1, sizeof(Note));
  expression->note->type = noteType;
  expression->note->octave = octave;
  return expression;
}

Program *KeyDefinitionSemanticAction(Expression *note, TokenLabel scaleType) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Program *program = calloc(1, sizeof(Program));
  program->type = KEY_DEFINITION;
  program->key = calloc(1, sizeof(KeyDefinition));
  program->key->note = note;
  program->key->scaleType = scaleType;
  return program;
}

Program *KeyDefinitionProgramSemanticAction(Program *keyDefinition) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  _compilerState->abstractSyntaxtTree = keyDefinition;
  return keyDefinition;
}

// New semantic actions for extended grammar

Program *StatementListProgramSemanticAction(Statement **statements) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Program *program = calloc(1, sizeof(Program));
  program->type = KEY_DEFINITION; // We'll use this as a generic program type
  program->statements = statements;
  _compilerState->abstractSyntaxtTree = program;
  return program;
}

Statement **SingleStatementListSemanticAction(Statement *statement) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement **statements = calloc(2, sizeof(Statement *));
  statements[0] = statement;
  statements[1] = NULL; // NULL terminator
  return statements;
}

Statement **MultipleStatementListSemanticAction(Statement **statements,
                                                Statement *statement) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  // Count existing statements
  int count = 0;
  while (statements[count] != NULL) {
    count++;
  }

  // Reallocate with space for one more statement
  Statement **newStatements =
      realloc(statements, (count + 2) * sizeof(Statement *));
  newStatements[count] = statement;
  newStatements[count + 1] = NULL; // NULL terminator
  return newStatements;
}

Statement *KeyDefinitionStatementSemanticAction(Program *keyDefinition) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = KEY_STATEMENT;
  statement->keyDefinition = keyDefinition->key;
  // Free the program wrapper since we're extracting the key definition
  free(keyDefinition);
  return statement;
}

Statement *TimeStatementSemanticAction(TimeSignature *timeSignature) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = TIME_STATEMENT;
  statement->timeSignature = timeSignature;
  return statement;
}

Statement *TempoStatementSemanticAction(TempoDeclaration *tempoDeclaration) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = TEMPO_STATEMENT;
  statement->tempoDeclaration = tempoDeclaration;
  return statement;
}

Statement *NotesStatementSemanticAction(NoteSequence *noteSequence) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = NOTES_STATEMENT;
  statement->noteSequence = noteSequence;
  return statement;
}

TimeSignature *TimeSignatureSemanticAction(int numerator, int denominator) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  TimeSignature *timeSignature = calloc(1, sizeof(TimeSignature));
  timeSignature->numerator = numerator;
  timeSignature->denominator = denominator;
  return timeSignature;
}

TempoDeclaration *TempoDeclarationSemanticAction(TokenLabel tempoName) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  TempoDeclaration *tempoDeclaration = calloc(1, sizeof(TempoDeclaration));

  // Convert token label to string
  const char *tempoNameStr;
  switch (tempoName) {
  case 274:
    tempoNameStr = "allegro";
    break; // ALLEGRO
  case 275:
    tempoNameStr = "andante";
    break; // ANDANTE
  case 276:
    tempoNameStr = "largo";
    break; // LARGO
  case 277:
    tempoNameStr = "presto";
    break; // PRESTO
  default:
    tempoNameStr = "unknown";
    break;
  }

  tempoDeclaration->tempoName = malloc(strlen(tempoNameStr) + 1);
  strcpy(tempoDeclaration->tempoName, tempoNameStr);
  return tempoDeclaration;
}

NoteSequence *SingleNoteSequenceSemanticAction(Note *note) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  NoteSequence *noteSequence = calloc(1, sizeof(NoteSequence));
  noteSequence->notes = calloc(2, sizeof(Note *));
  noteSequence->notes[0] = note;
  noteSequence->notes[1] = NULL; // NULL terminator
  noteSequence->count = 1;
  return noteSequence;
}

NoteSequence *MultipleNoteSequenceSemanticAction(NoteSequence *noteSequence,
                                                 Note *note) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  // Reallocate with space for one more note
  noteSequence->notes =
      realloc(noteSequence->notes, (noteSequence->count + 2) * sizeof(Note *));
  noteSequence->notes[noteSequence->count] = note;
  noteSequence->notes[noteSequence->count + 1] = NULL; // NULL terminator
  noteSequence->count++;
  return noteSequence;
}

Note *NoteWithDurationSemanticAction(Expression *note, NoteDuration duration) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Note *noteWithDuration = calloc(1, sizeof(Note));
  noteWithDuration->type = note->note->type;
  noteWithDuration->octave = note->note->octave;
  noteWithDuration->duration = duration;
  
  // Free the original note inside the expression
  free(note->note);
  // Free the expression wrapper
  free(note);
  
  return noteWithDuration;
}
