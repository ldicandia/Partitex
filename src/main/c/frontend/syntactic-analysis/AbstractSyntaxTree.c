#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

/** Shutdown module's internal state. */
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

/* PUBLIC FUNCTIONS */

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
      destroyKeyDefinition(program->key);
      break;
    default:
      printf("DEBUG: destroyProgram - Unknown program type: %d\n",
             program->type);
      break;
    }
    free(program);
  }
}