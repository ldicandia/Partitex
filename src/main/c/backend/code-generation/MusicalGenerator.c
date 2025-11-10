#include "MusicalGenerator.h"
#include "../domain-specific/MusicalCalculator.h"
#include <stdint.h>

/* MODULE INTERNAL STATE */

static const char _indentationCharacter = ' ';
static const char _indentationSize = 2;
static Logger *_logger = NULL;
static OutputFormat _outputFormat = OUTPUT_TEXT;

/** Shutdown module's internal state. */
void _shutdownMusicalGeneratorModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: MusicalGenerator...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeMusicalGeneratorModule() {
    _logger = createLogger("MusicalGenerator");
    return _shutdownMusicalGeneratorModule;
}

/** PRIVATE FUNCTIONS */

static char *_indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char *const format, ...);
static void _generateLilyPondPrologue(void);
static void _generateLilyPondEpilogue(void);
static void _generateTextPrologue(void);
static void _generateTextEpilogue(void);
static void _generateProgram(Program *program);
static void _generateStatement(Statement *statement, const unsigned int indentationLevel);
static void _generateNote(Note *note, const unsigned int indentationLevel);
static void _generateNoteSequence(NoteSequence *noteSequence, const unsigned int indentationLevel);
static void _generatePattern(Pattern *pattern, const unsigned int indentationLevel);
static void _generateMelody(Melody *melody, const unsigned int indentationLevel);
static void _generateSimultaneousNotes(SimultaneousNotes *simultaneousNotes, const unsigned int indentationLevel);
static void _generateRepeatStatement(RepeatStatement *repeatStatement, const unsigned int indentationLevel);
static void _generateKeyDefinition(KeyDefinition *keyDefinition, const unsigned int indentationLevel);
static void _generateTimeSignature(TimeSignature *timeSignature, const unsigned int indentationLevel);
static void _generateTempoDeclaration(TempoDeclaration *tempoDeclaration, const unsigned int indentationLevel);

/**
 * Generates an indentation string for the specified level.
 */
static char *_indentation(const unsigned int level) {
    return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output.
 */
static void _output(const unsigned int indentationLevel, const char *const format, ...) {
    va_list arguments;
    va_start(arguments, format);
    char *indentation = _indentation(indentationLevel);
    char *effectiveFormat = concatenate(2, indentation, format);
    vfprintf(stdout, effectiveFormat, arguments);
    fflush(stdout);
    free(effectiveFormat);
    free(indentation);
    va_end(arguments);
}

/**
 * Generates LilyPond prologue
 */
static void _generateLilyPondPrologue(void) {
    _output(0, "%s",
            "\\version \"2.24.0\"\n"
            "\\language \"english\"\n\n"
            "\\header {\n"
            "  title = \"Partitex Composition\"\n"
            "  composer = \"Partitex Compiler\"\n"
            "}\n\n"
            "\\score {\n"
            "  \\new Staff {\n");
}

/**
 * Generates LilyPond epilogue
 */
static void _generateLilyPondEpilogue(void) {
    _output(0, "%s",
            "  }\n"
            "  \\layout { }\n"
            "  \\midi { }\n"
            "}\n");
}

/**
 * Generates text prologue
 */
static void _generateTextPrologue(void) {
    _output(0, "%s",
            "========================================\n"
            "PARTITEX MUSICAL COMPOSITION\n"
            "========================================\n\n");
}

/**
 * Generates text epilogue
 */
static void _generateTextEpilogue(void) {
    _output(0, "%s",
            "\n========================================\n"
            "END OF COMPOSITION\n"
            "========================================\n");
}

/**
 * Generates a note in the appropriate format
 */
static void _generateNote(Note *note, const unsigned int indentationLevel) {
    if (note == NULL) return;
    
    const char* noteName = noteTypeToString(note->type);
    const char* duration = noteDurationToString(note->duration);
    
    // Safety check for NULL pointers
    if (noteName == NULL) noteName = "?";
    if (duration == NULL) duration = "?";
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "%s%s%s", noteName, "4", duration);
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "%s%s%s", noteName, "4/", duration);
            break;
    }
}

/**
 * Generates a note sequence
 */
static void _generateNoteSequence(NoteSequence *noteSequence, const unsigned int indentationLevel) {
    if (noteSequence == NULL || noteSequence->count == 0) return;
    
    for (int i = 0; i < noteSequence->count; i++) {
        if (noteSequence->notes[i] == NULL) break;
        _generateNote(noteSequence->notes[i], indentationLevel);
        if (i < noteSequence->count - 1) {
            // Print separator directly to avoid issues with _output
            if (_outputFormat == OUTPUT_LILYPOND) {
                fprintf(stdout, " ");
            } else {
                fprintf(stdout, ", ");
            }
            fflush(stdout);
        }
    }
}

/**
 * Generates a pattern
 */
static void _generatePattern(Pattern *pattern, const unsigned int indentationLevel) {
    if (pattern == NULL) return;
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "%% Pattern: %s\n", pattern->name);
            _generateNoteSequence(pattern->noteSequence, indentationLevel);
            _output(0, "%s", "\n");
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Pattern %s: ", pattern->name);
            _generateNoteSequence(pattern->noteSequence, 0);
            _output(0, "%s", "\n");
            break;
    }
}

/**
 * Generates a melody
 */
static void _generateMelody(Melody *melody, const unsigned int indentationLevel) {
    if (melody == NULL) return;
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "%% Melody: %s", melody->name);
            if (melody->duration != NULL) {
                _output(0, " (Duration: %d", melody->duration->value);
                switch (melody->duration->unit) {
                    case MINUTES: _output(0, "%s", " minutes"); break;
                    case SECONDS: _output(0, "%s", " seconds"); break;
                    case MILLISECONDS: _output(0, "%s", " ms"); break;
                }
                _output(0, "%s", ")");
            }
            _output(0, "%s", "\n");
            _generateNoteSequence(melody->noteSequence, indentationLevel);
            _output(0, "%s", "\n");
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Melody %s: ", melody->name);
            _generateNoteSequence(melody->noteSequence, 0);
            if (melody->duration != NULL) {
                _output(0, " (Duration: %d", melody->duration->value);
                switch (melody->duration->unit) {
                    case MINUTES: _output(0, "%s", " minutes"); break;
                    case SECONDS: _output(0, "%s", " seconds"); break;
                    case MILLISECONDS: _output(0, "%s", " ms"); break;
                }
                _output(0, "%s", ")");
            }
            _output(0, "%s", "\n");
            break;
    }
}

/**
 * Generates simultaneous notes
 */
static void _generateSimultaneousNotes(SimultaneousNotes *simultaneousNotes, const unsigned int indentationLevel) {
    if (simultaneousNotes == NULL) return;
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "%% Simultaneous instruments\n");
            _output(indentationLevel, "<<\n");
            for (int i = 0; i < simultaneousNotes->instrumentCount; i++) {
                _output(indentationLevel + 1, "\\new Staff { \\clef \"treble\"\n");
                _output(indentationLevel + 2, "%% %s\n", simultaneousNotes->instrumentNames[i]);
                _generateNoteSequence(simultaneousNotes->noteSequences[i], indentationLevel + 2);
                _output(0, "%s", "\n");
                _output(indentationLevel + 1, "}\n");
            }
            _output(indentationLevel, ">>\n");
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Simultaneous:\n");
            for (int i = 0; i < simultaneousNotes->instrumentCount; i++) {
                _output(indentationLevel + 1, "Instrument %s: ", simultaneousNotes->instrumentNames[i]);
                _generateNoteSequence(simultaneousNotes->noteSequences[i], 0);
                _output(0, "%s", "\n");
            }
            break;
    }
}

/**
 * Generates a repeat statement
 */
static void _generateRepeatStatement(RepeatStatement *repeatStatement, const unsigned int indentationLevel) {
    if (repeatStatement == NULL) return;
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "\\repeat volta %d {\n", repeatStatement->repeatCount);
            _output(indentationLevel + 1, "%% Pattern: %s\n", repeatStatement->patternName);
            if (repeatStatement->interval != NULL) {
                _output(indentationLevel + 1, "%% Interval: %d", repeatStatement->interval->value);
                switch (repeatStatement->interval->unit) {
                    case MINUTES: _output(0, "%s", " minutes"); break;
                    case SECONDS: _output(0, "%s", " seconds"); break;
                    case MILLISECONDS: _output(0, "%s", " ms"); break;
                }
                _output(0, "%s", "\n");
            }
            _output(indentationLevel, "}\n");
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Repeat %s %d times", repeatStatement->patternName, repeatStatement->repeatCount);
            if (repeatStatement->interval != NULL) {
                _output(0, " every %d", repeatStatement->interval->value);
                switch (repeatStatement->interval->unit) {
                    case MINUTES: _output(0, "%s", " minutes"); break;
                    case SECONDS: _output(0, "%s", " seconds"); break;
                    case MILLISECONDS: _output(0, "%s", " ms"); break;
                }
            }
            _output(0, "%s", "\n");
            break;
    }
}

/**
 * Generates a key definition
 */
static void _generateKeyDefinition(KeyDefinition *keyDefinition, const unsigned int indentationLevel) {
    if (keyDefinition == NULL || keyDefinition->note == NULL) return;
    
    // Extract the note from the expression
    const char* noteName = "C"; // Default
    if (keyDefinition->note->type == NOTE && keyDefinition->note->note != NULL) {
        noteName = noteTypeToString(keyDefinition->note->note->type);
    }
    
    // Determine scale type from the token label (274=MAJOR, 275=MINOR)
    const char* scaleType = (keyDefinition->scaleType == 274) ? "major" : "minor";
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "\\key %s \\%s\n", noteName, scaleType);
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Key: %s %s\n", noteName, scaleType);
            break;
    }
}

/**
 * Generates a time signature
 */
static void _generateTimeSignature(TimeSignature *timeSignature, const unsigned int indentationLevel) {
    if (timeSignature == NULL) return;
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "\\time %d/%d\n", timeSignature->numerator, timeSignature->denominator);
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Time signature: %d/%d\n", timeSignature->numerator, timeSignature->denominator);
            break;
    }
}

/**
 * Generates a tempo declaration
 */
static void _generateTempoDeclaration(TempoDeclaration *tempoDeclaration, const unsigned int indentationLevel) {
    if (tempoDeclaration == NULL) return;
    
    const char* tempoName = (tempoDeclaration->tempoName != NULL) ? tempoDeclaration->tempoName : "unknown";
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _output(indentationLevel, "\\tempo \"%s\"\n", tempoName);
            break;
        case OUTPUT_TEXT:
        default:
            _output(indentationLevel, "Tempo: %s\n", tempoName);
            break;
    }
}

/**
 * Generates a statement
 */
static void _generateStatement(Statement *statement, const unsigned int indentationLevel) {
    if (statement == NULL) return;
    
    switch (statement->type) {
        case KEY_STATEMENT:
            _generateKeyDefinition(statement->keyDefinition, indentationLevel);
            break;
        case TIME_STATEMENT:
            _generateTimeSignature(statement->timeSignature, indentationLevel);
            break;
        case TEMPO_STATEMENT:
            _generateTempoDeclaration(statement->tempoDeclaration, indentationLevel);
            break;
        case NOTES_STATEMENT:
            if (statement->noteSequence) {
                _output(indentationLevel, "Notes: ");
                _generateNoteSequence(statement->noteSequence, 0);
                fprintf(stdout, "\n");
                fflush(stdout);
            }
            break;
        case SIMULTANEOUS_STATEMENT:
            if (statement->simultaneousNotes) {
                _generateSimultaneousNotes(statement->simultaneousNotes, indentationLevel);
            }
            break;
        case PATTERN_STATEMENT:
            _generatePattern(statement->pattern, indentationLevel);
            break;
        case MELODY_STATEMENT:
            _generateMelody(statement->melody, indentationLevel);
            break;
        case REPEAT_STATEMENT:
            _generateRepeatStatement(statement->repeatStatement, indentationLevel);
            break;
        default:
            _output(indentationLevel, "Unknown statement type\n");
            break;
    }
}

/**
 * Generates the program
 */
static void _generateProgram(Program *program) {
    if (program == NULL) return;
    
    if (program->type == KEY_DEFINITION) {
        if (program->statements != NULL) {
            // Calculate statement count if not set (for safety)
            int statementCount = program->statementCount;
            if (statementCount == 0) {
                while (program->statements[statementCount] != NULL) {
                    statementCount++;
                }
            }
            
            for (int i = 0; i < statementCount; i++) {
                _generateStatement(program->statements[i], 0);
                if (i < statementCount - 1) {
                    _output(0, "%s", "\n");
                }
            }
        } else if (program->key) {
            _generateKeyDefinition(program->key, 0);
        }
    } else if (program->type == EXPRESSION_PROGRAM) {
        _output(0, "Mathematical expression: %d\n", program->expression ? 0 : 0);
    }
}

/** PUBLIC FUNCTIONS */

void setOutputFormat(OutputFormat format) {
    _outputFormat = format;
    logDebugging(_logger, "Output format set to: %d", format);
}

void executeMusicalGenerator(CompilerState *compilerState) {
    logDebugging(_logger, "Generating musical output...");
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _generateLilyPondPrologue();
            break;
        case OUTPUT_TEXT:
        default:
            _generateTextPrologue();
            break;
    }
    
    _generateProgram(compilerState->abstractSyntaxtTree);
    
    switch (_outputFormat) {
        case OUTPUT_LILYPOND:
            _generateLilyPondEpilogue();
            break;
        case OUTPUT_TEXT:
        default:
            _generateTextEpilogue();
            break;
    }
    
    logDebugging(_logger, "Musical generation is done.");
}
