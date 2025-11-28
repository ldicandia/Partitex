#include "MusicalGenerator.h"
#include "../domain-specific/MusicalCalculator.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



static const char _indentationCharacter = ' ';
static const char _indentationSize = 2;
static Logger *_logger = NULL;
static OutputFormat _outputFormat = OUTPUT_TEXT;

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
static void _generateMidiVisualization(NoteSequence *noteSequence, const unsigned int indentationLevel);
static int _getNoteIndex(NoteType noteType);
static int _getNoteIndexWithOctave(NoteType noteType, int octave, int minOctave);
static void _findOctaveRange(NoteSequence *noteSequence, int *minOctave, int *maxOctave);
static char** _generateNoteNames(int minOctave, int maxOctave, int *totalNotes);
static void _freeNoteNames(char** noteNames, int totalNotes);
static int _getNoteDurationUnits(NoteDuration duration);
static const char* _getNoteNameWithSharp(NoteType noteType);

static char *_indentation(const unsigned int level) {
    return indentation(_indentationCharacter, level, _indentationSize);
}

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

static void _generateLilyPondEpilogue(void) {
    _output(0, "%s",
            "  }\n"
            "  \\layout { }\n"
            "  \\midi { }\n"
            "}\n");
}

static void _generateTextPrologue(void) {
    _output(0, "%s",
            "========================================\n"
            "PARTITEX MUSICAL COMPOSITION\n"
            "========================================\n\n");
}

static void _generateTextEpilogue(void) {
    _output(0, "%s",
            "\n========================================\n"
            "END OF COMPOSITION\n"
            "========================================\n");
}

static void _generateNote(Note *note, const unsigned int indentationLevel) {
    if (note == NULL) return;
    
    const char* noteName = noteTypeToString(note->type);
    const char* duration = noteDurationToString(note->duration);
    
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

static void _findOctaveRange(NoteSequence *noteSequence, int *minOctave, int *maxOctave) {
    if (noteSequence == NULL || noteSequence->count == 0) {
        *minOctave = 4;
        *maxOctave = 4;
        return;
    }
    
    *minOctave = noteSequence->notes[0]->octave;
    *maxOctave = noteSequence->notes[0]->octave;
    
    for (int i = 0; i < noteSequence->count; i++) {
        if (noteSequence->notes[i] == NULL) break;
        int octave = noteSequence->notes[i]->octave;
        if (octave < *minOctave) *minOctave = octave;
        if (octave > *maxOctave) *maxOctave = octave;
    }
}

static int _getNoteIndexWithOctave(NoteType noteType, int octave, int minOctave) {
    int baseNoteIndex = _getNoteIndex(noteType);
    return (octave - minOctave) * 12 + baseNoteIndex;
}

static char** _generateNoteNames(int minOctave, int maxOctave, int *totalNotes) {
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    *totalNotes = (maxOctave - minOctave + 1) * 12;
    
    char** names = (char**)malloc(*totalNotes * sizeof(char*));
    if (names == NULL) return NULL;
    
    int index = 0;
    for (int octave = minOctave; octave <= maxOctave; octave++) {
        for (int note = 0; note < 12; note++) {
            names[index] = (char*)malloc(10 * sizeof(char));
            if (names[index] != NULL) {
                sprintf(names[index], "%s%d", noteNames[note], octave);
            }
            index++;
        }
    }
    
    return names;
}

static void _freeNoteNames(char** noteNames, int totalNotes) {
    if (noteNames == NULL) return;
    for (int i = 0; i < totalNotes; i++) {
        if (noteNames[i] != NULL) {
            free(noteNames[i]);
        }
    }
    free(noteNames);
}

static int _getNoteIndex(NoteType noteType) {
    switch (noteType) {
        case DO_NOTE: return 0;
        case RE_NOTE: return 2;
        case MI_NOTE: return 4;
        case FA_NOTE: return 5;
        case SOL_NOTE: return 7;
        case LA_NOTE: return 9;
        case SI_NOTE: return 11;
        default: return 0;
    }
}

static const char* _getNoteNameWithSharp(NoteType noteType) {
    switch (noteType) {
        case DO_NOTE: return "C";
        case RE_NOTE: return "D";
        case MI_NOTE: return "E";
        case FA_NOTE: return "F";
        case SOL_NOTE: return "G";
        case LA_NOTE: return "A";
        case SI_NOTE: return "B";
        default: return "?";
    }
}

static int _getNoteDurationUnits(NoteDuration duration) {
    switch (duration) {
        case WHOLE_NOTE: return 16;
        case HALF_NOTE: return 8;
        case QUARTER_NOTE: return 4;
        case EIGHTH_NOTE: return 2;
        case SIXTEENTH_NOTE: return 1;
        default: return 4;
    }
}

static void _generateMidiVisualization(NoteSequence *noteSequence, const unsigned int indentationLevel) {
    if (noteSequence == NULL || noteSequence->count == 0) return;
    
    int minOctave, maxOctave;
    _findOctaveRange(noteSequence, &minOctave, &maxOctave);
    
    int totalNotes;
    char** noteNames = _generateNoteNames(minOctave, maxOctave, &totalNotes);
    if (noteNames == NULL) return;
    
    // Calculate total time units
    int totalTime = 0;
    for (int i = 0; i < noteSequence->count; i++) {
        if (noteSequence->notes[i] == NULL) break;
        totalTime += _getNoteDurationUnits(noteSequence->notes[i]->duration);
    }
    
    // Create a grid: totalNotes x totalTime positions
    int** grid = (int**)malloc(totalNotes * sizeof(int*));
    for (int i = 0; i < totalNotes; i++) {
        grid[i] = (int*)calloc(totalTime, sizeof(int));
    }
    
    // Fill the grid with note positions
    int currentTime = 0;
    for (int i = 0; i < noteSequence->count; i++) {
        if (noteSequence->notes[i] == NULL) break;
        Note *note = noteSequence->notes[i];
        int noteIndex = _getNoteIndexWithOctave(note->type, note->octave, minOctave);
        int duration = _getNoteDurationUnits(note->duration);
        
        // Mark the note as active for its duration
        for (int t = 0; t < duration; t++) {
            if (currentTime + t < totalTime && noteIndex < totalNotes) {
                grid[noteIndex][currentTime + t] = 1;
            }
        }
        currentTime += duration;
    }
    
    // Print the visualization
    _output(indentationLevel, "%s", "\nMIDI Visualization:\n");
    for (int note = 0; note < totalNotes; note++) {
        // Allocate enough space for note name, tab, and all time positions
        int lineSize = totalTime + 20;
        char* line = (char*)malloc(lineSize * sizeof(char));
        if (line == NULL) continue;
        
        // Start with note name and tab
        int pos = sprintf(line, "%-4s\t", noteNames[note]);
        
        // Add the visualization characters
        for (int t = 0; t < totalTime && pos < lineSize - 1; t++) {
            line[pos++] = grid[note][t] ? '#' : '-';
        }
        line[pos] = '\0';
        
        _output(indentationLevel, "%s\n", line);
        free(line);
    }
    
    // Free allocated memory
    for (int i = 0; i < totalNotes; i++) {
        free(grid[i]);
    }
    free(grid);
    _freeNoteNames(noteNames, totalNotes);
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
    
    // Generate MIDI visualization for text output
    if (_outputFormat == OUTPUT_TEXT) {
        _generateMidiVisualization(noteSequence, indentationLevel);
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
