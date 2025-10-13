#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void _shutdownAbstractSyntaxTreeModule() {
  if (_logger != NULL) {
    logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
    destroyLogger(_logger);
    _logger = NULL;
  }
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
  _logger = createLogger("AbstractSyntaxTree");
  return _shutdownAbstractSyntaxTreeModule;
}

void destroyConstant(Constant *constant) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (constant != NULL) {
    free(constant);
  }
}

void destroyNote(Note *note) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (note != NULL) {
    free(note);
  }
}

void destroyExpression(Expression *expression) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (expression != NULL) {
    switch (expression->type) {
    case ADDITION:
    case DIVISION:
    case MULTIPLICATION:
    case SUBTRACTION:
      destroyExpression(expression->leftExpression);
      destroyExpression(expression->rightExpression);
      break;
    case FACTOR:
      destroyFactor(expression->factor);
      break;
    case NOTE:
      destroyNote(expression->note);
      break;
    }
    free(expression);
  }
}

void destroyFactor(Factor *factor) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (factor != NULL) {
    switch (factor->type) {
    case CONSTANT:
      destroyConstant(factor->constant);
      break;
    case EXPRESSION:
      destroyExpression(factor->expression);
      break;
    }
    free(factor);
  }
}

void destroyKeyDefinition(KeyDefinition *keyDefinition) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (keyDefinition != NULL) {
    logDebugging(_logger, "Destroying KeyDefinition at %p\n",
                 (void *)keyDefinition);
    destroyExpression(keyDefinition->note);
    free(keyDefinition);
  }
}

void destroyTimeSignature(TimeSignature *timeSignature) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (timeSignature != NULL) {
    free(timeSignature);
  }
}

void destroyTempoDeclaration(TempoDeclaration *tempoDeclaration) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (tempoDeclaration != NULL) {
    if (tempoDeclaration->tempoName != NULL) {
      free(tempoDeclaration->tempoName);
    }
    free(tempoDeclaration);
  }
}

void destroyNoteSequence(NoteSequence *noteSequence) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (noteSequence != NULL) {
    if (noteSequence->notes != NULL) {
      for (int i = 0; i < noteSequence->count; i++) {
        destroyNote(noteSequence->notes[i]);
      }
      free(noteSequence->notes);
    }
    free(noteSequence);
  }
}

void destroyPattern(Pattern *pattern) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (pattern != NULL) {
    if (pattern->name != NULL) {
      free(pattern->name);
    }
    destroyNoteSequence(pattern->noteSequence);
    free(pattern);
  }
}

void destroyMelody(Melody *melody) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (melody != NULL) {
    if (melody->name != NULL) {
      free(melody->name);
    }
    destroyNoteSequence(melody->noteSequence);
    if (melody->duration != NULL) {
      destroyTimeValue(melody->duration);
    }
    free(melody);
  }
}

void destroyRepeatStatement(RepeatStatement *repeatStatement) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (repeatStatement != NULL) {
    if (repeatStatement->patternName != NULL) {
      free(repeatStatement->patternName);
    }
    if (repeatStatement->interval != NULL) {
      destroyTimeValue(repeatStatement->interval);
    }
    free(repeatStatement);
  }
}

void destroySimultaneousNotes(SimultaneousNotes *simultaneousNotes) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (simultaneousNotes != NULL) {
    if (simultaneousNotes->noteSequences != NULL) {
      for (int i = 0; i < simultaneousNotes->instrumentCount; i++) {
        destroyNoteSequence(simultaneousNotes->noteSequences[i]);
      }
      free(simultaneousNotes->noteSequences);
    }
    if (simultaneousNotes->instrumentNames != NULL) {
      for (int i = 0; i < simultaneousNotes->instrumentCount; i++) {
        free(simultaneousNotes->instrumentNames[i]);
      }
      free(simultaneousNotes->instrumentNames);
    }
    free(simultaneousNotes);
  }
}

void destroyTimeValue(TimeValue *timeValue) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (timeValue != NULL) {
    free(timeValue);
  }
}

void destroyStatement(Statement *statement) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (statement != NULL) {
    switch (statement->type) {
    case KEY_STATEMENT:
      destroyKeyDefinition(statement->keyDefinition);
      break;
    case TIME_STATEMENT:
      destroyTimeSignature(statement->timeSignature);
      break;
    case TEMPO_STATEMENT:
      destroyTempoDeclaration(statement->tempoDeclaration);
      break;
    case NOTES_STATEMENT:
      if (statement->noteSequence != NULL) {
        destroyNoteSequence(statement->noteSequence);
      } else if (statement->simultaneousNotes != NULL) {
        destroySimultaneousNotes(statement->simultaneousNotes);
      }
      break;
    case PATTERN_STATEMENT:
      destroyPattern(statement->pattern);
      break;
    case MELODY_STATEMENT:
      destroyMelody(statement->melody);
      break;
    case REPEAT_STATEMENT:
      destroyRepeatStatement(statement->repeatStatement);
      break;
    }
    free(statement);
  }
}

void destroyProgram(Program *program) {
  logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
  if (program != NULL) {
    printf("DEBUG: destroyProgram - Program type is %d (KEY_DEFINITION=%d, "
           "EXPRESSION_PROGRAM=%d)\n",
           program->type, KEY_DEFINITION, EXPRESSION_PROGRAM);
    switch (program->type) {
    case EXPRESSION_PROGRAM:
      printf("DEBUG: destroyProgram - Calling destroyExpression\n");
      destroyExpression(program->expression);
      break;
    case KEY_DEFINITION:
      printf("DEBUG: destroyProgram - Calling destroyKeyDefinition\n");
      if (program->statements != NULL) {
        int statementCount = 0;
        for (int i = 0; program->statements[i] != NULL; i++) {
          statementCount++;
        }
        for (int i = 0; program->statements[i] != NULL; i++) {
          destroyStatement(program->statements[i]);
        }
        free(program->statements);
      } else {
        destroyKeyDefinition(program->key);
      }
      break;
    default:
      printf("DEBUG: destroyProgram - Unknown program type: %d\n",
             program->type);
      break;
    }
    free(program);
  }
}