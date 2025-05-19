#ifndef FLEX_ACTIONS_H
#define FLEX_ACTIONS_H

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

// Token definitions (estos valores deben coincidir con los del parser)
// #define OPEN_BRACES 256
// #define CLOSE_BRACES 257
// #define OPEN_BRACKET 258
// #define CLOSE_BRACKET 259
// #define OPEN_PAREN 260
// #define CLOSE_PAREN 261
// #define QUOTE 262
// #define COLON 263
// #define COMMA 264

// #define METHODS 265
// #define METHOD 266
// #define PARAMS 267
// #define DESCRIPTION 268
// #define TYPE 269
// #define STYLE 270
// #define TITLE 271
// #define RANGE 272
// #define REGEX 273
// #define RELATED 274

// #define TRUE 275
// #define FALSE 276
// #define NULL_VALUE 277

#define STRING 278
#define INTEGER_NUMBER 279
#define FLOAT_NUMBER 280
#define IDENTIFIER 281

#define UNKNOWN_TOKEN 282

/**
 * Structure to hold lexical analyzer context information.
 */
// typedef struct
// {
//     char *lexeme; // The actual text of the token
//     int length;   // Length of the lexeme
//     int line;     // Line number where token appears
//     int column;   // Column number where token starts
// } LexicalAnalyzerContext;

/**
 * Union for storing different types of token values.
 * This should match the %union definition in your bison file.
 */
// typedef union
// {
//     int token;       // For simple tokens
//     char *string;    // For string and identifier values
//     int integer;     // For integer numbers
//     double floating; // For floating point numbers
// } YYSTYPE;

// // Global yylval variable (defined by bison/yacc)
// extern YYSTYPE yylval;

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

// Function prototypes
// LexicalAnalyzerContext createLexicalAnalyzerContext(void);

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext *context);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext *context);
void IgnoredLexemeAction(LexicalAnalyzerContext *context);

int ParenthesisLexemeAction(LexicalAnalyzerContext *context, int token);
int KeywordLexemeAction(LexicalAnalyzerContext *context, int token);
int StringLexemeAction(LexicalAnalyzerContext *context, int token);
int NumberLexemeAction(LexicalAnalyzerContext *context, int token);
int IdentifierLexemeAction(LexicalAnalyzerContext *context, int token);
int UnknownLexemeAction(LexicalAnalyzerContext *context);

// Utility functions
const char *getTokenName(int token);
void printLexicalAnalysisStatistics(void);
void resetLexicalAnalysisCounters(void);

// Debug flag - define this to enable debug output
// #define DEBUG_LEXER

#endif // FLEX_ACTIONS_H