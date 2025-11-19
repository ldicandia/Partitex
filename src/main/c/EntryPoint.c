#include "backend/code-generation/Generator.h"
#include "backend/code-generation/MusicalGenerator.h"
#include "backend/domain-specific/Calculator.h"
#include "backend/domain-specific/MusicalCalculator.h"
#include "frontend/Frontend.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "support/logging/Logger.h"
#include "support/type/CompilationStatus.h"
#include "support/type/CompilerState.h"
#include "support/type/ModuleDestructor.h"

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int length, const char **arguments) {
  LexicalAnalyzer *lexicalAnalyzer = createLexicalAnalyzer();
  Logger *logger = createLogger("EntryPoint");
  for (int k = 0; k < length; ++k) {
    logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
  }
  CompilerState compilerState = {.abstractSyntaxtTree = NULL, .value = 0};
  ModuleDestructor moduleDestructors[] = {
      initializeAbstractSyntaxTreeModule(),
      initializeFlexActionsModule(lexicalAnalyzer),
      initializeBisonActionsModule(&compilerState),
      initializeFrontendModule(lexicalAnalyzer),
      initializeCalculatorModule(),
      initializeMusicalCalculatorModule(),
      initializeGeneratorModule(),
      initializeMusicalGeneratorModule()};
  CompilationStatus compilationStatus = executeSyntacticAnalysis();
  Program *program = compilerState.abstractSyntaxtTree;
  if (compilationStatus == SUCCEEDED) {
    // ----------------------------------------------------------------------------------------
    // Beginning of the Backend...
    // ------------------------------------------------------------
    logDebugging(logger, "Processing musical composition...");
    
    // Check if this is a musical program or mathematical expression
    if (program->type == KEY_DEFINITION) {
      // Musical program - use musical backend
      MusicalComputationResult musicalResult = executeMusicalCalculator(&compilerState);
      if (musicalResult.succeeded) {
        logDebugging(logger, "Musical computation succeeded");
        executeMusicalGenerator(&compilerState);
      } else {
        logError(logger, "The musical computation phase rejects the input program.");
        compilationStatus = FAILED;
      }
      // Free musical result
      if (musicalResult.result != NULL) {
        free(musicalResult.result);
      }
    } else if (program->type == EXPRESSION_PROGRAM) {
      // Mathematical expression - use original calculator
      logDebugging(logger, "Computing mathematical expression value...");
      ComputationResult computationResult = executeCalculator(&compilerState);
      if (computationResult.succeeded) {
        compilerState.value = computationResult.value;
        executeGenerator(&compilerState);
      } else {
        logError(logger, "The computation phase rejects the input program.");
        compilationStatus = FAILED;
      }
    } else {
      logError(logger, "Unknown program type: %d", program->type);
      compilationStatus = FAILED;
    }
    // ...end of the Backend.
    // -----------------------------------------------------------------
    // ----------------------------------------------------------------------------------------
  } else {
    logError(logger, "The syntactic-analysis phase rejects the input program.");
    compilationStatus = FAILED;
  }
  logDebugging(logger, "Releasing AST resources...");
  if (program != NULL) {
    destroyProgram(program);
  }
  for (int k = (sizeof(moduleDestructors) / sizeof(ModuleDestructor)) - 1;
       0 <= k; --k) {
    moduleDestructors[k]();
  }
  logDebugging(logger, "Compilation is done.");
  destroyLogger(logger);
  destroyLexicalAnalyzer(lexicalAnalyzer);
  return compilationStatus;
}
