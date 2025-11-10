#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static bool _logIgnoredLexemes = true;
static InputBuffer *_inputBuffer = NULL;
static LexicalAnalyzer *_lexicalAnalyzer = NULL;
static Logger *_logger = NULL;

/** Shutdown module's internal state. */
void _shutdownFlexActionsModule() {
  if (_logger != NULL) {
    logDebugging(_logger, "Destroying module: FlexActions...");
    destroyLogger(_logger);
    _logger = NULL;
  }
  if (_inputBuffer != NULL) {
    destroyInputBuffer(_inputBuffer);
    _inputBuffer = NULL;
  }
  _lexicalAnalyzer = NULL;
}

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer *lexicalAnalyzer) {
  _inputBuffer = NULL;
  _lexicalAnalyzer = lexicalAnalyzer;
  _logger = createLogger("FlexActions");
  _logIgnoredLexemes =
      getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
  return _shutdownFlexActionsModule;
}

static void _logTokenAction(const char *actionName, Token *token);

static void _logTokenAction(const char *actionName, Token *token) {
  char *_lexeme = escape(token->lexeme);
  logDebugging(_logger,
               WARNING_COLOR "%s" DEFAULT_COLOR
                             ": Token(context=%d, label=%d, length=%d, "
                             "lexeme=%s\"%s\"%s, line=%d, semanticValue=%p)",
               actionName, token->context, token->label, token->length,
               INFORMATION_COLOR, _lexeme, DEFAULT_COLOR, token->line,
               token->semanticValue);
  free(_lexeme);
  _lexeme = NULL;
}

CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label) {
  Token *token = createToken(_lexicalAnalyzer, label);
  _logTokenAction(__FUNCTION__, token);
  CompilationStatus status = pushToken(_lexicalAnalyzer, token);
  destroyToken(token);
  return status;
}

CompilationStatus EnterImportExpressionLexemeAction(FlexContext context) {
  if (_logIgnoredLexemes) {
    Token *token = createToken(_lexicalAnalyzer, OPEN_BRACE);
    _logTokenAction(__FUNCTION__, token);
    destroyToken(token);
  }
  enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
  return IN_PROGRESS;
}

CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context) {
  if (_logIgnoredLexemes) {
    Token *token = createToken(_lexicalAnalyzer, OPEN_COMMENT);
    _logTokenAction(__FUNCTION__, token);
    destroyToken(token);
  }
  enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
  return IN_PROGRESS;
}

CompilationStatus EOFLexemeAction() {
  CompilationStatus status = IN_PROGRESS;
  Token *token = createToken(_lexicalAnalyzer, 0);
  _logTokenAction(__FUNCTION__, token);
  if (!popInputBuffer(_lexicalAnalyzer)) {
    status = pushToken(_lexicalAnalyzer, token);
    FlexContext context = currentLexicalAnalyzerContext(_lexicalAnalyzer);
    if (0 < context) {
      logError(_logger, "The final context is not closed (context=%d).",
               context);
      status = FAILED;
    }
  }
  destroyToken(token);
  return status;
}

CompilationStatus IgnoredLexemeAction() {
  if (_logIgnoredLexemes) {
    Token *token = createToken(_lexicalAnalyzer, IGNORED);
    _logTokenAction(__FUNCTION__, token);
    destroyToken(token);
  }
  return IN_PROGRESS;
}

CompilationStatus IntegerLexemeAction() {
  Token *token = createToken(_lexicalAnalyzer, INTEGER);
  token->semanticValue->integer = atoi(token->lexeme);
  _logTokenAction(__FUNCTION__, token);
  CompilationStatus status = pushToken(_lexicalAnalyzer, token);
  destroyToken(token);
  return status;
}

CompilationStatus LeaveImportExpressionLexemeAction() {
  pushInputBuffer(_inputBuffer);
  leaveLexicalAnalyzerContext(_lexicalAnalyzer);
  if (_logIgnoredLexemes) {
    Token *token = createToken(_lexicalAnalyzer, CLOSE_BRACE);
    _logTokenAction(__FUNCTION__, token);
    destroyToken(token);
  }
  return IN_PROGRESS;
}

CompilationStatus LeaveMultilineCommentLexemeAction() {
  leaveLexicalAnalyzerContext(_lexicalAnalyzer);
  if (_logIgnoredLexemes) {
    Token *token = createToken(_lexicalAnalyzer, CLOSE_COMMENT);
    _logTokenAction(__FUNCTION__, token);
    destroyToken(token);
  }
  return IN_PROGRESS;
}

CompilationStatus ParenthesisLexemeAction(TokenLabel label) {
  Token *token = createToken(_lexicalAnalyzer, label);
  _logTokenAction(__FUNCTION__, token);
  CompilationStatus status = pushToken(_lexicalAnalyzer, token);
  destroyToken(token);
  return status;
}

CompilationStatus SubexpressionLexemeAction() {
  Token *token = createToken(_lexicalAnalyzer, IGNORED);
  _inputBuffer = createInputBuffer(_lexicalAnalyzer, token->lexeme);
  if (_logIgnoredLexemes) {
    _logTokenAction(__FUNCTION__, token);
  }
  destroyToken(token);
  return IN_PROGRESS;
}

CompilationStatus UnknownLexemeAction() {
  Token *token = createToken(_lexicalAnalyzer, UNKNOWN);
  _logTokenAction(__FUNCTION__, token);
  destroyToken(token);
  return FAILED;
}

CompilationStatus TokenLexemeAction(TokenLabel label) {
  Token *token = createToken(_lexicalAnalyzer, label);
  _logTokenAction(__FUNCTION__, token);
  CompilationStatus status = pushToken(_lexicalAnalyzer, token);
  destroyToken(token);
  return status;
}

CompilationStatus IdentifierLexemeAction() {
  Token *token = createToken(_lexicalAnalyzer, IDENTIFIER);
  char *identifierString = malloc(strlen(token->lexeme) + 1);
  if (identifierString != NULL) {
    strcpy(identifierString, token->lexeme);
    token->semanticValue->string = identifierString;
  } else {
    token->semanticValue->string = NULL;
  }
  _logTokenAction(__FUNCTION__, token);
  CompilationStatus status = pushToken(_lexicalAnalyzer, token);
  destroyToken(token);
  return status;
}

CompilationStatus NoteWithOctaveLexemeAction(NoteType noteType, int octave) {
  Token *token = createToken(_lexicalAnalyzer, NOTE_TOKEN);
  token->semanticValue->note = calloc(1, sizeof(Note));
  if (token->semanticValue->note != NULL) {
    token->semanticValue->note->type = noteType;
    token->semanticValue->note->octave = octave;
  }
  _logTokenAction(__FUNCTION__, token);
  CompilationStatus status = pushToken(_lexicalAnalyzer, token);
  destroyToken(token);
  return status;
}
