#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * Forward declarations for self-referencing types
 */
typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Note Note;
typedef struct KeyDefinition KeyDefinition;
typedef struct Program Program;

/**
 * Enumeration types
 */
typedef enum {
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
  FACTOR,
  NOTE
} ExpressionType;

typedef enum { CONSTANT, EXPRESSION } FactorType;

typedef enum {
  DO_NOTE,
  RE_NOTE,
  MI_NOTE,
  FA_NOTE,
  SOL_NOTE,
  LA_NOTE,
  SI_NOTE
} NoteType;

typedef enum { EXPRESSION_PROGRAM, KEY_DEFINITION } ProgramType;

/**
 * AST node structures
 */
struct Constant {
  int value;
};

struct Factor {
  union {
    Constant *constant;
    Expression *expression;
  };
  FactorType type;
};

struct Note {
  NoteType type;
  int octave;
};

struct Expression {
  ExpressionType type;
  union {
    struct {
      Expression *leftExpression;
      Expression *rightExpression;
    };
    Factor *factor;
    Note *note;
  };
};

struct KeyDefinition {
  Expression *note;
  TokenLabel scaleType; // MAJOR o MINOR
};

struct Program {
  ProgramType type;
  union {
    Expression *expression;
    KeyDefinition *key;
  };
};

/**
 * Node destructors
 */
void destroyConstant(Constant *constant);
void destroyExpression(Expression *expression);
void destroyFactor(Factor *factor);
void destroyProgram(Program *program);
void destroyNote(Note *note);
void destroyKeyDefinition(KeyDefinition *keyDefinition);

#endif