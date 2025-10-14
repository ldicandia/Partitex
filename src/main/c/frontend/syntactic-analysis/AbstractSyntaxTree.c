#include "AbstractSyntaxTree.h"

static Logger *_logger = NULL;

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

void destroyNote(Note *note) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (note != NULL) {
        if (note->name) free(note->name);
        free(note);
    }
}

void destroyKeyDefinition(KeyDefinition *key_definition) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (key_definition != NULL) {
        destroyNote(key_definition->note);
        free(key_definition);
    }
}

void destroyTimeSignature(TimeSignature *time_signature) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (time_signature != NULL) {
        free(time_signature);
    }
}

void destroyNoteSequence(NoteSequence *note_sequence) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (note_sequence != NULL) {
        for (int i = 0; i < note_sequence->count; i++) {
            destroyStatement(note_sequence->statements[i]);
        }
        free(note_sequence->statements);
        free(note_sequence);
    }
}

void destroyPattern(Pattern *pattern) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (pattern != NULL) {
        if (pattern->name) free(pattern->name);
        destroyNoteSequence(pattern->note_sequence);
        free(pattern);
    }
}

void destroyStatement(Statement *statement) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statement != NULL) {
        switch (statement->type) {
            case KEY_STATEMENT:
                destroyKeyDefinition(statement->key_definition);
                break;
            case TIME_STATEMENT:
                destroyTimeSignature(statement->time_signature);
                break;
            case CLEF_STATEMENT:
                // TokenLabel, no alloc
                break;
            case NOTE_STATEMENT:
                destroyNote(statement->note);
                break;
            case SET_STATEMENT:
                // TokenLabel, no alloc
                break;
            case REST_STEP_STATEMENT:
                // No alloc
                break;
            case PATTERN_STATEMENT:
                destroyPattern(statement->pattern);
                break;
            case REPEAT_STATEMENT:
                if (statement->repeat.pattern_name) free(statement->repeat.pattern_name);
                break;
        }
        free(statement);
    }
}

void destroyProgram(Program *program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program != NULL) {
        if (program->def_statements) {
            for (int i = 0; program->def_statements[i] != NULL; i++) {
                destroyStatement(program->def_statements[i]);
            }
            free(program->def_statements);
        }
        if (program->notes_statements) {
            for (int i = 0; program->notes_statements[i] != NULL; i++) {
                destroyStatement(program->notes_statements[i]);
            }
            free(program->notes_statements);
        }
        free(program);
    }
}