#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include <stdlib.h>

ModuleDestructor initializeAbstractSyntaxTreeModule();

typedef struct Program Program;
typedef struct KeyDefinition KeyDefinition;
typedef struct TimeSignature TimeSignature;
typedef struct NoteSequence NoteSequence;
typedef struct Note Note;
typedef struct Pattern Pattern;
typedef struct Statement Statement;

typedef enum {
    DO_NOTE, RE_NOTE, MI_NOTE, FA_NOTE, SOL_NOTE, LA_NOTE, SI_NOTE
} NoteType;

typedef enum {
    NONE_ALT, BEMOL, SHARP
} AlterationType;

typedef enum {
    NONE_ART, STACCATO, LEGATO, ACCENT
} ArticulationType;

typedef enum {
    NONE_DUR, WHOLE, HALF, QUARTER, EIGHTH, SIXTEENTH, THIRTYSECOND, SIXTYFOURTH
} NoteDuration;

typedef enum {
    KEY_STATEMENT, TIME_STATEMENT, CLEF_STATEMENT, NOTE_STATEMENT, SET_STATEMENT, REST_STEP_STATEMENT, PATTERN_STATEMENT, REPEAT_STATEMENT
} StatementType;

struct Note {
    NoteType type;
    char *name; // "do", "re", etc.
    int octave;
    AlterationType alteration;
    NoteDuration duration;
    ArticulationType articulation;
};

struct KeyDefinition {
    Note *note;
    TokenLabel scale_type;
};

struct TimeSignature {
    int numerator;
    int denominator;
};

struct NoteSequence {
    Statement **statements;
    int count;
};

struct Pattern {
    char *name; // NULL for main
    NoteSequence *note_sequence;
};

struct Statement {
    StatementType type;
    union {
        KeyDefinition *key_definition;
        TimeSignature *time_signature;
        TokenLabel clef;
        Note *note;
        TokenLabel setting; // For SET (tempo or dynamics)
        struct { TokenLabel type; int number; } rest_step;
        Pattern *pattern;
        struct { char *pattern_name; int repeat_count; } repeat;
    };
};

struct Program {
    Statement **def_statements;
    Statement **notes_statements;
    int main_count;
};

void destroyProgram(Program *program);
void destroyKeyDefinition(KeyDefinition *key_definition);
void destroyTimeSignature(TimeSignature *time_signature);
void destroyNoteSequence(NoteSequence *note_sequence);
void destroyNote(Note *note);
void destroyPattern(Pattern *pattern);
void destroyStatement(Statement *statement);

#endif