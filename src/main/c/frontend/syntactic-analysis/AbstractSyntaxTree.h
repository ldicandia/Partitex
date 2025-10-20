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
typedef struct Pattern Pattern;
typedef struct Melody Melody;
typedef struct RepeatStatement RepeatStatement;
typedef struct SimultaneousNotes SimultaneousNotes;
typedef struct TimeValue TimeValue;
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
  NOTES_STATEMENT,
  SIMULTANEOUS_STATEMENT,
  PATTERN_STATEMENT,
  REPEAT_STATEMENT,
  MELODY_STATEMENT
} StatementType;

typedef enum {
  WHOLE_NOTE,
  HALF_NOTE,
  QUARTER_NOTE,
  EIGHTH_NOTE,
  SIXTEENTH_NOTE
} NoteDuration;

typedef enum { SECONDS, MILLISECONDS, MINUTES } TimeUnit;

typedef enum { NOTE_TYPE, MELODY_TYPE, PATTERN_TYPE } MusicalDataType;

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

struct TimeValue {
  int value;
  TimeUnit unit;
};

struct Pattern {
  char *name;
  NoteSequence *noteSequence;
  MusicalDataType dataType;
};

struct Melody {
  char *name;
  NoteSequence *noteSequence;
  TimeValue *duration;
};

struct RepeatStatement {
  char *patternName;
  int repeatCount;
  TimeValue *interval; // Optional interval between repetitions
};

struct SimultaneousNotes {
  NoteSequence **noteSequences; // Array of note sequences for different instruments
  int instrumentCount;
  char **instrumentNames; // Names of instruments
};

struct Statement {
  StatementType type;
  union {
    KeyDefinition *keyDefinition;
    TimeSignature *timeSignature;
    TempoDeclaration *tempoDeclaration;
    NoteSequence *noteSequence;
    Pattern *pattern;
    RepeatStatement *repeatStatement;
    Melody *melody;
    SimultaneousNotes *simultaneousNotes;
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
void destroyPattern(Pattern *pattern);
void destroyMelody(Melody *melody);
void destroyRepeatStatement(RepeatStatement *repeatStatement);
void destroySimultaneousNotes(SimultaneousNotes *simultaneousNotes);
void destroyTimeValue(TimeValue *timeValue);
void destroyStatement(Statement *statement);
void destroyStatementList(Statement **list);
void destroyProgram(Program *program);
void destroyPattern(Pattern *pattern);
void destroyMelody(Melody *melody);
void destroyRepeatStatement(RepeatStatement *repeatStatement);
void destroyKeyDefinition(KeyDefinition *keyDef);


#endif