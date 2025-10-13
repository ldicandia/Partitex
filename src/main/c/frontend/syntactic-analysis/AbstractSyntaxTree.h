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
typedef struct TimeSignature TimeSignature;
typedef struct TempoDeclaration TempoDeclaration;
typedef struct NoteSequence NoteSequence;
typedef struct Statement Statement;
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

typedef enum {
  KEY_STATEMENT,
  TIME_STATEMENT,
  TEMPO_STATEMENT,
  NOTES_STATEMENT
} StatementType;

typedef enum {
  WHOLE_NOTE,
  HALF_NOTE,
  QUARTER_NOTE,
  EIGHTH_NOTE,
  SIXTEENTH_NOTE
} NoteDuration;

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
  NoteDuration duration;
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

struct TimeSignature {
  int numerator;
  int denominator;
};

struct TempoDeclaration {
  char *tempoName; // e.g., "allegro"
};

struct NoteSequence {
  Note **notes;
  int count;
};

struct Statement {
  StatementType type;
  union {
    KeyDefinition *keyDefinition;
    TimeSignature *timeSignature;
    TempoDeclaration *tempoDeclaration;
    NoteSequence *noteSequence;
  };
};

struct Program {
  ProgramType type;
  union {
    Expression *expression;
    KeyDefinition *key;
    Statement **statements; // For multiple statements
  };
  int statementCount; // Number of statements if using statements array
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
void destroyTimeSignature(TimeSignature *timeSignature);
void destroyTempoDeclaration(TempoDeclaration *tempoDeclaration);
void destroyNoteSequence(NoteSequence *noteSequence);
void destroyStatement(Statement *statement);

#endif