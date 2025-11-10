#include "MusicalCalculator.h"
#include "../../support/language/String.h"
#include <string.h>
#include <stdlib.h>

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

/** Shutdown module's internal state. */
void _shutdownMusicalCalculatorModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: MusicalCalculator...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeMusicalCalculatorModule() {
    _logger = createLogger("MusicalCalculator");
    return _shutdownMusicalCalculatorModule;
}

/** PRIVATE FUNCTIONS */

static MusicalComputationResult _createSuccessResult(const char* result);
static MusicalComputationResult _createFailureResult();
static void _freeMusicalResult(MusicalComputationResult *result);

/**
 * Creates a successful computation result with the given string.
 */
static MusicalComputationResult _createSuccessResult(const char* result) {
    MusicalComputationResult computationResult = {
        .succeeded = true,
        .result = result != NULL ? strdup(result) : NULL
    };
    return computationResult;
}

/**
 * Creates a failed computation result.
 */
static MusicalComputationResult _createFailureResult() {
    MusicalComputationResult computationResult = {
        .succeeded = false,
        .result = NULL
    };
    return computationResult;
}

/**
 * Frees the memory allocated for a musical computation result.
 */
static void _freeMusicalResult(MusicalComputationResult *result) {
    if (result != NULL && result->result != NULL) {
        free(result->result);
        result->result = NULL;
    }
}

/** PUBLIC FUNCTIONS */

const char* noteTypeToString(NoteType noteType) {
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

const char* noteDurationToString(NoteDuration duration) {
    switch (duration) {
        case WHOLE_NOTE: return "1";
        case HALF_NOTE: return "2";
        case QUARTER_NOTE: return "4";
        case EIGHTH_NOTE: return "8";
        case SIXTEENTH_NOTE: return "16";
        default: return "4";
    }
}

const char* tempoNameToBPM(const char* tempoName) {
    if (tempoName == NULL) return "120";
    
    if (strcmp(tempoName, "largo") == 0) return "60";
    if (strcmp(tempoName, "adagio") == 0) return "70";
    if (strcmp(tempoName, "andante") == 0) return "80";
    if (strcmp(tempoName, "moderato") == 0) return "100";
    if (strcmp(tempoName, "allegro") == 0) return "120";
    if (strcmp(tempoName, "presto") == 0) return "160";
    if (strcmp(tempoName, "prestissimo") == 0) return "200";
    
    return "120"; // Default tempo
}

int calculateNoteDuration(NoteDuration duration, int bpm, int timeSignatureNumerator, int timeSignatureDenominator) {
    // Calculate duration in milliseconds
    int beatsPerSecond = bpm / 60;
    int millisecondsPerBeat = 1000 / beatsPerSecond;
    
    // Adjust for time signature
    double noteValue = 1.0;
    switch (duration) {
        case WHOLE_NOTE: noteValue = 4.0; break;
        case HALF_NOTE: noteValue = 2.0; break;
        case QUARTER_NOTE: noteValue = 1.0; break;
        case EIGHTH_NOTE: noteValue = 0.5; break;
        case SIXTEENTH_NOTE: noteValue = 0.25; break;
    }
    
    // Adjust for time signature denominator
    noteValue = noteValue * (4.0 / timeSignatureDenominator);
    
    return (int)(noteValue * millisecondsPerBeat);
}

MusicalComputationResult computeNote(Note *note, MusicalContext *context) {
    if (note == NULL) {
        logError(_logger, "Cannot compute note: note is NULL");
        return _createFailureResult();
    }
    
    const char* noteName = noteTypeToString(note->type);
    const char* duration = noteDurationToString(note->duration);
    
    // Create note string in format: "C4/4" (note + octave + duration)
    char* octaveStr = concatenate(2, "4", "/");
    char* noteString = concatenate(3, noteName, octaveStr, duration);
    free(octaveStr);
    
    logDebugging(_logger, "Computed note: %s", noteString);
    return _createSuccessResult(noteString);
}

MusicalComputationResult computeNoteSequence(NoteSequence *noteSequence, MusicalContext *context) {
    if (noteSequence == NULL) {
        logError(_logger, "Cannot compute note sequence: sequence is NULL");
        return _createFailureResult();
    }
    
    if (noteSequence->count == 0) {
        return _createSuccessResult("");
    }
    
    char* result = strdup("");
    
    for (int i = 0; i < noteSequence->count; i++) {
        MusicalComputationResult noteResult = computeNote(noteSequence->notes[i], context);
        if (!noteResult.succeeded) {
            _freeMusicalResult(&noteResult);
            free(result);
            return _createFailureResult();
        }
        
        char* newResult;
        if (i == 0) {
            newResult = noteResult.result;
        } else {
            newResult = concatenate(3, result, ", ", noteResult.result);
            free(result);
            free(noteResult.result);
        }
        result = newResult;
    }
    
    logDebugging(_logger, "Computed note sequence: %s", result);
    return _createSuccessResult(result);
}

MusicalComputationResult computePattern(Pattern *pattern, MusicalContext *context) {
    if (pattern == NULL) {
        logError(_logger, "Cannot compute pattern: pattern is NULL");
        return _createFailureResult();
    }
    
    MusicalComputationResult sequenceResult = computeNoteSequence(pattern->noteSequence, context);
    if (!sequenceResult.succeeded) {
        return sequenceResult;
    }
    
    char* result = concatenate(3, "Pattern ", pattern->name, ": ");
    char* newResult = concatenate(2, result, sequenceResult.result);
    free(result);
    free(sequenceResult.result);
    
    logDebugging(_logger, "Computed pattern: %s", newResult);
    return _createSuccessResult(newResult);
}

MusicalComputationResult computeMelody(Melody *melody, MusicalContext *context) {
    if (melody == NULL) {
        logError(_logger, "Cannot compute melody: melody is NULL");
        return _createFailureResult();
    }
    
    MusicalComputationResult sequenceResult = computeNoteSequence(melody->noteSequence, context);
    if (!sequenceResult.succeeded) {
        return sequenceResult;
    }
    
    char* durationStr = "";
    if (melody->duration != NULL) {
        char* unit = melody->duration->unit == MINUTES ? "min" : 
                    melody->duration->unit == SECONDS ? "sec" : "ms";
        char* valueStr = concatenate(2, "1", " ");
        char* durationPart = concatenate(3, valueStr, unit, ")");
        durationStr = concatenate(2, " (", durationPart);
        free(valueStr);
        free(durationPart);
    }
    
    char* result = concatenate(4, "Melody ", melody->name, ": ", sequenceResult.result);
    char* newResult = concatenate(2, result, durationStr);
    free(result);
    free(sequenceResult.result);
    
    logDebugging(_logger, "Computed melody: %s", newResult);
    return _createSuccessResult(newResult);
}

MusicalComputationResult computeSimultaneousNotes(SimultaneousNotes *simultaneousNotes, MusicalContext *context) {
    if (simultaneousNotes == NULL) {
        logError(_logger, "Cannot compute simultaneous notes: notes is NULL");
        return _createFailureResult();
    }
    
    char* result = strdup("Simultaneous:\n");
    
    for (int i = 0; i < simultaneousNotes->instrumentCount; i++) {
        MusicalComputationResult sequenceResult = computeNoteSequence(simultaneousNotes->noteSequences[i], context);
        if (!sequenceResult.succeeded) {
            _freeMusicalResult(&sequenceResult);
            free(result);
            return _createFailureResult();
        }
        
        char* instrumentLine = concatenate(4, "  ", simultaneousNotes->instrumentNames[i], ": ", sequenceResult.result);
        char* newResult = concatenate(3, result, instrumentLine, "\n");
        free(result);
        free(instrumentLine);
        free(sequenceResult.result);
        result = newResult;
    }
    
    logDebugging(_logger, "Computed simultaneous notes");
    return _createSuccessResult(result);
}

MusicalComputationResult computeRepeatStatement(RepeatStatement *repeatStatement, MusicalContext *context) {
    if (repeatStatement == NULL) {
        logError(_logger, "Cannot compute repeat statement: statement is NULL");
        return _createFailureResult();
    }
    
    char* intervalStr = "";
    if (repeatStatement->interval != NULL) {
        char* unit = repeatStatement->interval->unit == MINUTES ? "min" : 
                    repeatStatement->interval->unit == SECONDS ? "sec" : "ms";
        char* valueStr = concatenate(2, "1", " ");
        char* intervalPart = concatenate(2, valueStr, unit);
        intervalStr = concatenate(2, " every ", intervalPart);
        free(valueStr);
        free(intervalPart);
    }
    
    char* timesStr = concatenate(2, "2", " times");
    char* result = concatenate(3, "Repeat ", repeatStatement->patternName, " ");
    char* newResult = concatenate(3, result, timesStr, intervalStr);
    free(result);
    free(timesStr);
    
    logDebugging(_logger, "Computed repeat statement: %s", newResult);
    return _createSuccessResult(newResult);
}

MusicalComputationResult executeMusicalCalculator(CompilerState *compilerState) {
    if (compilerState == NULL || compilerState->abstractSyntaxtTree == NULL) {
        logError(_logger, "Cannot execute musical calculator: invalid compiler state");
        return _createFailureResult();
    }
    
    Program *program = compilerState->abstractSyntaxtTree;
    MusicalContext context = {0};
    
    // Initialize default context
    context.currentBPM = 120; // Default tempo
    
    char* result = strdup("Musical Composition:\n");
    
    if (program->type == KEY_DEFINITION) {
        if (program->statements != NULL) {
            // Calculate statement count if not set (for safety)
            int statementCount = program->statementCount;
            if (statementCount == 0) {
                while (program->statements[statementCount] != NULL) {
                    statementCount++;
                }
            }
            
            // Process multiple statements
            for (int i = 0; i < statementCount; i++) {
                Statement *statement = program->statements[i];
                if (statement == NULL) continue;
                
                MusicalComputationResult statementResult = {0};
                
                switch (statement->type) {
                    case KEY_STATEMENT:
                        context.currentKey = statement->keyDefinition;
                        statementResult = _createSuccessResult("Key definition processed");
                        break;
                    case TIME_STATEMENT:
                        context.currentTimeSignature = statement->timeSignature;
                        statementResult = _createSuccessResult("Time signature processed");
                        break;
                    case TEMPO_STATEMENT:
                        context.currentTempo = statement->tempoDeclaration;
                        if (statement->tempoDeclaration && statement->tempoDeclaration->tempoName) {
                            context.currentBPM = atoi(tempoNameToBPM(statement->tempoDeclaration->tempoName));
                        }
                        statementResult = _createSuccessResult("Tempo processed");
                        break;
                    case NOTES_STATEMENT:
                        if (statement->noteSequence) {
                            statementResult = computeNoteSequence(statement->noteSequence, &context);
                        } else if (statement->simultaneousNotes) {
                            statementResult = computeSimultaneousNotes(statement->simultaneousNotes, &context);
                        }
                        break;
                    case PATTERN_STATEMENT:
                        statementResult = computePattern(statement->pattern, &context);
                        break;
                    case MELODY_STATEMENT:
                        statementResult = computeMelody(statement->melody, &context);
                        break;
                    case REPEAT_STATEMENT:
                        statementResult = computeRepeatStatement(statement->repeatStatement, &context);
                        break;
                    default:
                        statementResult = _createSuccessResult("Unknown statement type");
                        break;
                }
                
                if (statementResult.succeeded && statementResult.result) {
                    char* newResult = concatenate(3, result, "  ", statementResult.result);
                    free(result);
                    result = newResult;
                    if (i < statementCount - 1) {
                        char* newResult2 = concatenate(2, result, "\n");
                        free(result);
                        result = newResult2;
                    }
                }
                _freeMusicalResult(&statementResult);
            }
        } else if (program->key) {
            context.currentKey = program->key;
            result = concatenate(2, result, "Key definition processed");
        }
    } else if (program->type == EXPRESSION_PROGRAM) {
        // Handle mathematical expressions (fallback to original calculator)
        result = concatenate(2, result, "Mathematical expression processed");
    }
    
    logDebugging(_logger, "Musical calculation completed successfully");
    return _createSuccessResult(result);
}
