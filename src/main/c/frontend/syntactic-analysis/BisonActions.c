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

Expression *NoteFromTokenSemanticAction(Note *note) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Expression *expression = calloc(1, sizeof(Expression));
  expression->type = NOTE;
  expression->note = note;
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

Program *StatementListProgramSemanticAction(Statement **statements) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Program *program = calloc(1, sizeof(Program));
  program->type = KEY_DEFINITION;
  program->statements = statements;
  
  // Count statements (array ends with NULL)
  int count = 0;
  if (statements != NULL) {
    while (statements[count] != NULL) {
      count++;
    }
  }
  program->statementCount = count;
  
  _compilerState->abstractSyntaxtTree = program;
  return program;
}

Statement **SingleStatementListSemanticAction(Statement *statement) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement **statements = calloc(2, sizeof(Statement *));
  statements[0] = statement;
  statements[1] = NULL;
  return statements;
}

Statement **MultipleStatementListSemanticAction(Statement **statements,
                                                Statement *statement) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  int count = 0;
  while (statements[count] != NULL) {
    count++;
  }

  Statement **newStatements =
      realloc(statements, (count + 2) * sizeof(Statement *));
  if (newStatements == NULL) {
    if (statement != NULL) {
      destroyStatement(statement);
    }
    return statements;
  }

  statements = newStatements;
  statements[count] = statement;
  statements[count + 1] = NULL;
  return statements;
}

Statement *KeyDefinitionStatementSemanticAction(Program *keyDefinition) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = KEY_STATEMENT;
  statement->keyDefinition = keyDefinition->key;
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

  const char *tempoNameStr;
  switch (tempoName) {
  case 294:
    tempoNameStr = "allegro";
    break; // ALLEGRO
  case 295:
    tempoNameStr = "andante";
    break; // ANDANTE
  case 296:
    tempoNameStr = "largo";
    break; // LARGO
  case 297:
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
  noteSequence->notes[1] = NULL;
  noteSequence->count = 1;
  return noteSequence;
}

NoteSequence *MultipleNoteSequenceSemanticAction(NoteSequence *noteSequence,
                                                 Note *note) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Note **newNotes =
      realloc(noteSequence->notes, (noteSequence->count + 2) * sizeof(Note *));
  if (newNotes == NULL) {
    if (note != NULL) {
      destroyNote(note);
    }
    return noteSequence;
  }

  noteSequence->notes = newNotes;
  noteSequence->notes[noteSequence->count] = note;
  noteSequence->notes[noteSequence->count + 1] = NULL;
  noteSequence->count++;
  return noteSequence;
}

Note *NoteWithDurationSemanticAction(Expression *note, NoteDuration duration) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Note *noteWithDuration = calloc(1, sizeof(Note));
  noteWithDuration->type = note->note->type;
  noteWithDuration->octave = note->note->octave;
  noteWithDuration->duration = duration;

  free(note->note);
  free(note);

  return noteWithDuration;
}

Statement *PatternStatementSemanticAction(char *name,
                                          NoteSequence *noteSequence) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = PATTERN_STATEMENT;
  statement->pattern = calloc(1, sizeof(Pattern));
  statement->pattern->name = malloc(strlen(name) + 1);
  strcpy(statement->pattern->name, name);
  statement->pattern->noteSequence = noteSequence;
  statement->pattern->dataType = PATTERN_TYPE;
  free(name);
  return statement;
}

Statement *MelodyStatementSemanticAction(char *name, NoteSequence *noteSequence,
                                         TimeValue *duration) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = MELODY_STATEMENT;
  statement->melody = calloc(1, sizeof(Melody));
  statement->melody->name = malloc(strlen(name) + 1);
  strcpy(statement->melody->name, name);
  statement->melody->noteSequence = noteSequence;
  statement->melody->duration = duration;
  free(name);
  return statement;
}

Statement *RepeatStatementSemanticAction(char *patternName, int repeatCount,
                                         TimeValue *interval) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = REPEAT_STATEMENT;
  statement->repeatStatement = calloc(1, sizeof(RepeatStatement));
  statement->repeatStatement->patternName = malloc(strlen(patternName) + 1);
  strcpy(statement->repeatStatement->patternName, patternName);
  statement->repeatStatement->repeatCount = repeatCount;
  statement->repeatStatement->interval = interval;
  free(patternName);
  return statement;
}

Statement *
SimultaneousStatementSemanticAction(SimultaneousNotes *simultaneousNotes) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  Statement *statement = calloc(1, sizeof(Statement));
  statement->type = SIMULTANEOUS_STATEMENT;
  statement->simultaneousNotes = simultaneousNotes;
  return statement;
}

SimultaneousNotes *
SingleSimultaneousSemanticAction(char *instrumentName,
                                 NoteSequence *noteSequence) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  SimultaneousNotes *simultaneous = calloc(1, sizeof(SimultaneousNotes));
  simultaneous->instrumentCount = 1;
  simultaneous->noteSequences = calloc(2, sizeof(NoteSequence *));
  simultaneous->noteSequences[0] = noteSequence;
  simultaneous->noteSequences[1] = NULL;
  simultaneous->instrumentNames = calloc(2, sizeof(char *));
  simultaneous->instrumentNames[0] = malloc(strlen(instrumentName) + 1);
  strcpy(simultaneous->instrumentNames[0], instrumentName);
  simultaneous->instrumentNames[1] = NULL;
  free(instrumentName);
  return simultaneous;
}

SimultaneousNotes *
MultipleSimultaneousSemanticAction(SimultaneousNotes *existing,
                                   char *instrumentName,
                                   NoteSequence *noteSequence) {
  _logSyntacticAnalyzerAction(__FUNCTION__);

  NoteSequence **newNoteSequences =
      realloc(existing->noteSequences,
              (existing->instrumentCount + 2) * sizeof(NoteSequence *));
  char **newInstrumentNames =
      realloc(existing->instrumentNames,
              (existing->instrumentCount + 2) * sizeof(char *));

  if (newNoteSequences == NULL || newInstrumentNames == NULL) {
    if (newNoteSequences != NULL &&
        newNoteSequences != existing->noteSequences) {
      free(newNoteSequences);
    }
    if (newInstrumentNames != NULL &&
        newInstrumentNames != existing->instrumentNames) {
      free(newInstrumentNames);
    }

    if (noteSequence != NULL) {
      destroyNoteSequence(noteSequence);
    }
    if (instrumentName != NULL) {
      free(instrumentName);
    }
    return existing;
  }

  existing->noteSequences = newNoteSequences;
  existing->instrumentNames = newInstrumentNames;

  existing->noteSequences[existing->instrumentCount] = noteSequence;
  existing->noteSequences[existing->instrumentCount + 1] = NULL;

  existing->instrumentNames[existing->instrumentCount] =
      malloc(strlen(instrumentName) + 1);
  if (existing->instrumentNames[existing->instrumentCount] == NULL) {
    if (noteSequence != NULL) {
      destroyNoteSequence(noteSequence);
    }
    free(instrumentName);
    return existing;
  }

  strcpy(existing->instrumentNames[existing->instrumentCount], instrumentName);
  free(instrumentName);
  existing->instrumentNames[existing->instrumentCount + 1] = NULL;

  existing->instrumentCount++;
  return existing;
}

TimeValue *TimeValueSemanticAction(int value, TimeUnit unit) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  TimeValue *timeValue = calloc(1, sizeof(TimeValue));
  timeValue->value = value;
  timeValue->unit = unit;
  return timeValue;
}

char *IdentifierSemanticAction(TokenLabel token) {
  _logSyntacticAnalyzerAction(__FUNCTION__);
  return NULL;
}
