#ifndef MUSICAL_CALCULATOR_HEADER
#define MUSICAL_CALCULATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeMusicalCalculatorModule();

/**
 * The result of a musical computation. It's considered valid only if "succeeded" is true.
 */
typedef struct {
    bool succeeded;
    char *result; // Musical output as string
} MusicalComputationResult;

/**
 * Musical context for processing
 */
typedef struct {
    KeyDefinition *currentKey;
    TimeSignature *currentTimeSignature;
    TempoDeclaration *currentTempo;
    int currentBPM; // Beats per minute
} MusicalContext;

/**
 * Computes the final value of a musical note.
 */
MusicalComputationResult computeNote(Note *note, MusicalContext *context);

/**
 * Computes the final value of a note sequence.
 */
MusicalComputationResult computeNoteSequence(NoteSequence *noteSequence, MusicalContext *context);

/**
 * Computes the final value of a pattern.
 */
MusicalComputationResult computePattern(Pattern *pattern, MusicalContext *context);

/**
 * Computes the final value of a melody.
 */
MusicalComputationResult computeMelody(Melody *melody, MusicalContext *context);

/**
 * Computes the final value of simultaneous notes.
 */
MusicalComputationResult computeSimultaneousNotes(SimultaneousNotes *simultaneousNotes, MusicalContext *context);

/**
 * Computes the final value of a repeat statement.
 */
MusicalComputationResult computeRepeatStatement(RepeatStatement *repeatStatement, MusicalContext *context);

/**
 * Computes the program value using the current compiler state.
 */
MusicalComputationResult executeMusicalCalculator(CompilerState *compilerState);

/**
 * Helper functions for musical processing
 */
const char* noteTypeToString(NoteType noteType);
const char* noteDurationToString(NoteDuration duration);
const char* tempoNameToBPM(const char* tempoName);
int calculateNoteDuration(NoteDuration duration, int bpm, int timeSignatureNumerator, int timeSignatureDenominator);

#endif
