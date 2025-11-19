#ifndef MUSICAL_GENERATOR_HEADER
#define MUSICAL_GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdarg.h>
#include <stdio.h>

/** Initialize module's internal state. */
ModuleDestructor initializeMusicalGeneratorModule();

/**
 * Generates the final musical output using the current compiler state.
 */
void executeMusicalGenerator(CompilerState *compilerState);

/**
 * Output format options
 */
typedef enum {
    OUTPUT_LILYPOND,    // LilyPond notation
    OUTPUT_MIDI,        // MIDI format
    OUTPUT_ABC,         // ABC notation
    OUTPUT_TEXT         // Plain text representation
} OutputFormat;

/**
 * Set the output format for the generator
 */
void setOutputFormat(OutputFormat format);

#endif
