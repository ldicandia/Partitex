#include "BisonActions.h"
#include <string.h>

static CompilerState *_compilerState = NULL;
static Logger *_logger = NULL;
static int main_count = 0;

void _shutdownBisonActionsModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: BisonActions...");
        destroyLogger(_logger);
        _logger = NULL;
    }
    _compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState *compilerState) {
    _compilerState = compilerState;
    _logger = createLogger("BisonActions");
    return _shutdownBisonActionsModule;
}

static void _logSyntacticAnalyzerAction(const char *functionName) {
    logDebugging(_logger, "%s", functionName);
}

Program *ProgramSemanticAction(Program *definitions_section, Program *notes) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program *program = calloc(1, sizeof(Program));
    program->def_statements = definitions_section ? definitions_section->def_statements : NULL;
    program->notes_statements = notes ? notes->notes_statements : NULL;
    program->main_count = main_count;
    if (main_count != 1) {
        yyerror(NULL, "Error: exactly one main block required");
    }
    _compilerState->abstractSyntaxtTree = program;
    if (definitions_section) free(definitions_section);
    if (notes) free(notes);
    return program;
}

Program *DefinitionsSectionSemanticAction(Statement **definitions) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program *program = calloc(1, sizeof(Program));
    program->def_statements = definitions;
    return program;
}

Statement **AddDefinitionSemanticAction(Statement *definition, Statement **definitions) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    int count = 0;
    if (definitions) {
        while (definitions[count] != NULL) count++;
    }
    Statement **new_definitions = realloc(definitions, (count + 2) * sizeof(Statement *));
    if (!new_definitions) {
        destroyStatement(definition);
        return definitions;
    }
    new_definitions[count] = definition;
    new_definitions[count + 1] = NULL;
    return new_definitions;
}

KeyDefinition *KeyDefinitionSemanticAction(Note *note, TokenLabel scale_type) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    KeyDefinition *key_definition = calloc(1, sizeof(KeyDefinition));
    key_definition->note = note;
    key_definition->scale_type = scale_type;
    return key_definition;
}

Statement *TimeStatementSemanticAction(TimeSignature *time_signature) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = TIME_STATEMENT;
    statement->time_signature = time_signature;
    return statement;
}

Statement *ClefStatementSemanticAction(TokenLabel clef) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = CLEF_STATEMENT;
    statement->clef = clef;
    return statement;
}

TimeSignature *TimeSignatureSemanticAction(int numerator, int denominator) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TimeSignature *time_signature = calloc(1, sizeof(TimeSignature));
    time_signature->numerator = numerator;
    time_signature->denominator = denominator;
    return time_signature;
}

Program *NotesStatementSemanticAction(Statement **functions) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program *program = calloc(1, sizeof(Program));
    program->notes_statements = functions;
    return program;
}

Statement **AddFunctionSemanticAction(Pattern *function, Statement **functions) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = PATTERN_STATEMENT;
    statement->pattern = function;
    int count = 0;
    if (functions) {
        while (functions[count] != NULL) count++;
    }
    Statement **new_functions = realloc(functions, (count + 2) * sizeof(Statement *));
    if (!new_functions) {
        destroyStatement(statement);
        return functions;
    }
    new_functions[count] = statement;
    new_functions[count + 1] = NULL;
    return new_functions;
}

Pattern *MainFunctionSemanticAction(Statement **lines) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    main_count++;
    Pattern *pattern = calloc(1, sizeof(Pattern));
    pattern->name = NULL; // Main has no name
    pattern->note_sequence = calloc(1, sizeof(NoteSequence));
    pattern->note_sequence->statements = lines;
    pattern->note_sequence->count = lines ? (int)(sizeof(lines)/sizeof(lines[0])) : 0;
    return pattern;
}

Pattern *PatternFunctionSemanticAction(char *name, Statement **lines) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Pattern *pattern = calloc(1, sizeof(Pattern));
    pattern->name = name;
    pattern->note_sequence = calloc(1, sizeof(NoteSequence));
    pattern->note_sequence->statements = lines;
    pattern->note_sequence->count = lines ? (int)(sizeof(lines)/sizeof(lines[0])) : 0;
    return pattern;
}

Statement **SingleLineSemanticAction(Note *note, TokenLabel bemol_sharp, TokenLabel figure, TokenLabel articulation, Statement **lines) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    note->alteration = (bemol_sharp == BEMOL) ? BEMOL : (bemol_sharp == SHARP) ? SHARP : NONE_ALT;
    note->duration = figure ? figure : QUARTER;
    note->articulation = (articulation == STACCATO) ? STACCATO : (articulation == LEGATO) ? LEGATO : (articulation == ACCENT) ? ACCENT : NONE_ART;
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = NOTE_STATEMENT;
    statement->note = note;
    int count = 0;
    if (lines) {
        while (lines[count] != NULL) count++;
    }
    Statement **new_lines = realloc(lines, (count + 2) * sizeof(Statement *));
    if (!new_lines) {
        destroyStatement(statement);
        return lines;
    }
    new_lines[count] = statement;
    new_lines[count + 1] = NULL;
    return new_lines;
}

Statement **SetConfigurationSemanticAction(TokenLabel configuration, Statement **lines) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = SET_STATEMENT;
    statement->setting = configuration;
    int count = 0;
    if (lines) {
        while (lines[count] != NULL) count++;
    }
    Statement **new_lines = realloc(lines, (count + 2) * sizeof(Statement *));
    if (!new_lines) {
        destroyStatement(statement);
        return lines;
    }
    new_lines[count] = statement;
    new_lines[count + 1] = NULL;
    return new_lines;
}

Statement **RestStepSemanticAction(TokenLabel type, int number, Statement **lines) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = REST_STEP_STATEMENT;
    statement->rest_step.type = type;
    statement->rest_step.number = number;
    int count = 0;
    if (lines) {
        while (lines[count] != NULL) count++;
    }
    Statement **new_lines = realloc(lines, (count + 2) * sizeof(Statement *));
    if (!new_lines) {
        destroyStatement(statement);
        return lines;
    }
    new_lines[count] = statement;
    new_lines[count + 1] = NULL;
    return new_lines;
}

Statement **FunctionCallSemanticAction(char *identifier, int quantifier, Statement **lines) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement *statement = calloc(1, sizeof(Statement));
    statement->type = REPEAT_STATEMENT;
    statement->repeat.pattern_name = identifier;
    statement->repeat.repeat_count = quantifier;
    int count = 0;
    if (lines) {
        while (lines[count] != NULL) count++;
    }
    Statement **new_lines = realloc(lines, (count + 2) * sizeof(Statement *));
    if (!new_lines) {
        destroyStatement(statement);
        return lines;
    }
    new_lines[count] = statement;
    new_lines[count + 1] = NULL;
    return new_lines;
}

Note *NoteOctaveSemanticAction(Note *note, int octave) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    note->octave = octave;
    return note;
}

Note *NoteSemanticAction(NoteType note_type) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Note *note = calloc(1, sizeof(Note));
    note->type = note_type;
    switch (note_type) {
        case DO_NOTE: note->name = strdup("do"); break;
        case RE_NOTE: note->name = strdup("re"); break;
        case MI_NOTE: note->name = strdup("mi"); break;
        case FA_NOTE: note->name = strdup("fa"); break;
        case SOL_NOTE: note->name = strdup("sol"); break;
        case LA_NOTE: note->name = strdup("la"); break;
        case SI_NOTE: note->name = strdup("si"); break;
    }
    return note;
}