#ifndef FLEX_ACTIONS_H
#define FLEX_ACTIONS_H

// Token definitions (estos valores deben coincidir con los del parser)
#define OPEN_BRACES 256
#define CLOSE_BRACES 257
#define OPEN_BRACKET 258
#define CLOSE_BRACKET 259
#define OPEN_PAREN 260
#define CLOSE_PAREN 261
#define COLON 262
#define COMMA 263

#define METHODS 264
#define METHOD 265
#define PARAMS 266
#define DESCRIPTION 267
#define TYPE 268
#define STYLE 269
#define TITLE 270
#define RANGE 271
#define REGEX 272
#define RELATED 273

#define TRUE 274
#define FALSE 275
#define NULL_VALUE 276

#define STRING 277
#define INTEGER_NUMBER 278
#define FLOAT_NUMBER 279
#define IDENTIFIER 280

#define UNKNOWN_TOKEN 281

/**
 * Structure to hold lexical analyzer context information.
 */
typedef struct {
    char* lexeme;        // The actual text of the token
    int length;          // Length of the lexeme
    int line;            // Line number where token appears
    int column;          // Column number where token starts
} LexicalAnalyzerContext;

/**
 * Union for storing different types of token values.
 * This should match the %union definition in your bison file.
 */
typedef union {
    int token;           // For simple tokens
    char* string;        // For string and identifier values
    int integer;         // For integer numbers
    double floating;     // For floating point numbers
} YYSTYPE;

// Global yylval variable (defined by bison/yacc)
extern YYSTYPE yylval;

// Function prototypes
LexicalAnalyzerContext createLexicalAnalyzerContext(void);

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext context);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext context);
void IgnoredLexemeAction(LexicalAnalyzerContext context);

int ParenthesisLexemeAction(LexicalAnalyzerContext context, int token);
int KeywordLexemeAction(LexicalAnalyzerContext context, int token);
int StringLexemeAction(LexicalAnalyzerContext context, int token);
int NumberLexemeAction(LexicalAnalyzerContext context, int token);
int IdentifierLexemeAction(LexicalAnalyzerContext context, int token);
int UnknownLexemeAction(LexicalAnalyzerContext context);

// Utility functions
const char* getTokenName(int token);
void printLexicalAnalysisStatistics(void);
void resetLexicalAnalysisCounters(void);

// Debug flag - define this to enable debug output
// #define DEBUG_LEXER

#endif // FLEX_ACTIONS_H