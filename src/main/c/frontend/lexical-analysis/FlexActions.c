#include "FlexActions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External variables from flex
extern char* yytext;
extern int yylineno;
extern int yyleng;

// Global variables for tracking lexical analysis
static int commentDepth = 0;
static int totalTokens = 0;
static int totalCharacters = 0;

/**
 * Creates a lexical analyzer context with current state information.
 */
LexicalAnalyzerContext createLexicalAnalyzerContext() {
    LexicalAnalyzerContext context;
    context.lexeme = yytext;
    context.length = yyleng;
    context.line = yylineno;
    context.column = totalCharacters - yyleng + 1; // Approximate column calculation
    return context;
}

/**
 * Handles the beginning of multiline comments.
 */
void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext context) {
    commentDepth++;
    totalCharacters += context.length;
    
    #ifdef DEBUG_LEXER
    printf("[DEBUG] Begin multiline comment at line %d, column %d\n", 
           context.line, context.column);
    #endif
}

/**
 * Handles the end of multiline comments.
 */
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext context) {
    if (commentDepth > 0) {
        commentDepth--;
    }
    totalCharacters += context.length;
    
    #ifdef DEBUG_LEXER
    printf("[DEBUG] End multiline comment at line %d, column %d\n", 
           context.line, context.column);
    #endif
}

/**
 * Handles ignored lexemes (whitespace, comments, etc.).
 */
void IgnoredLexemeAction(LexicalAnalyzerContext context) {
    totalCharacters += context.length;
    
    #ifdef DEBUG_LEXER
    printf("[DEBUG] Ignored lexeme '%.*s' at line %d\n", 
           context.length, context.lexeme, context.line);
    #endif
}

/**
 * Handles parentheses and structural tokens.
 */
int ParenthesisLexemeAction(LexicalAnalyzerContext context, int token) {
    totalTokens++;
    totalCharacters += context.length;
    
    // Store lexeme value in yylval if needed
    yylval.token = token;
    
    #ifdef DEBUG_LEXER
    const char* tokenName = getTokenName(token);
    printf("[DEBUG] Parenthesis token: %s ('%.*s') at line %d, column %d\n", 
           tokenName, context.length, context.lexeme, context.line, context.column);
    #endif
    
    return token;
}

/**
 * Handles keyword tokens.
 */
int KeywordLexemeAction(LexicalAnalyzerContext context, int token) {
    totalTokens++;
    totalCharacters += context.length;
    
    // Store lexeme value
    yylval.token = token;
    
    #ifdef DEBUG_LEXER
    const char* tokenName = getTokenName(token);
    printf("[DEBUG] Keyword token: %s ('%.*s') at line %d, column %d\n", 
           tokenName, context.length, context.lexeme, context.line, context.column);
    #endif
    
    return token;
}

/**
 * Handles string literals.
 */
int StringLexemeAction(LexicalAnalyzerContext context, int token) {
    totalTokens++;
    totalCharacters += context.length;
    
    // Allocate memory for the string (removing quotes)
    int stringLength = context.length - 2; // Remove quotes
    char* stringValue = (char*)malloc(stringLength + 1);
    
    if (stringValue == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for string at line %d\n", context.line);
        exit(EXIT_FAILURE);
    }
    
    // Copy string content without quotes and process escape sequences
    int src = 1; // Skip opening quote
    int dst = 0;
    
    while (src < context.length - 1) { // Skip closing quote
        if (context.lexeme[src] == '\\' && src + 1 < context.length - 1) {
            // Handle escape sequences
            src++; // Skip backslash
            switch (context.lexeme[src]) {
                case 'n': stringValue[dst++] = '\n'; break;
                case 't': stringValue[dst++] = '\t'; break;
                case 'r': stringValue[dst++] = '\r'; break;
                case 'b': stringValue[dst++] = '\b'; break;
                case 'f': stringValue[dst++] = '\f'; break;
                case '"': stringValue[dst++] = '"'; break;
                case '\\': stringValue[dst++] = '\\'; break;
                case '/': stringValue[dst++] = '/'; break;
                default:
                    // Unknown escape sequence, keep as is
                    stringValue[dst++] = '\\';
                    stringValue[dst++] = context.lexeme[src];
                    break;
            }
        } else {
            stringValue[dst++] = context.lexeme[src];
        }
        src++;
    }
    stringValue[dst] = '\0';
    
    // Store in yylval
    yylval.string = stringValue;
    
    #ifdef DEBUG_LEXER
    printf("[DEBUG] String token: \"%s\" at line %d, column %d\n", 
           stringValue, context.line, context.column);
    #endif
    
    return token;
}

/**
 * Handles numeric literals.
 */
int NumberLexemeAction(LexicalAnalyzerContext context, int token) {
    totalTokens++;
    totalCharacters += context.length;
    
    // Convert string to number
    if (token == INTEGER_NUMBER) {
        yylval.integer = atoi(context.lexeme);
        
        #ifdef DEBUG_LEXER
        printf("[DEBUG] Integer token: %d at line %d, column %d\n", 
               yylval.integer, context.line, context.column);
        #endif
    } else if (token == FLOAT_NUMBER) {
        yylval.floating = atof(context.lexeme);
        
        #ifdef DEBUG_LEXER
        printf("[DEBUG] Float token: %f at line %d, column %d\n", 
               yylval.floating, context.line, context.column);
        #endif
    }
    
    return token;
}

/**
 * Handles identifier tokens.
 */
int IdentifierLexemeAction(LexicalAnalyzerContext context, int token) {
    totalTokens++;
    totalCharacters += context.length;
    
    // Allocate memory for identifier
    char* identifier = (char*)malloc(context.length + 1);
    if (identifier == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for identifier at line %d\n", context.line);
        exit(EXIT_FAILURE);
    }
    
    strncpy(identifier, context.lexeme, context.length);
    identifier[context.length] = '\0';
    
    yylval.string = identifier;
    
    #ifdef DEBUG_LEXER
    printf("[DEBUG] Identifier token: %s at line %d, column %d\n", 
           identifier, context.line, context.column);
    #endif
    
    return token;
}

/**
 * Handles unknown or invalid tokens.
 */
int UnknownLexemeAction(LexicalAnalyzerContext context) {
    totalCharacters += context.length;
    
    fprintf(stderr, "Lexical error: Unknown character '%c' at line %d, column %d\n", 
            context.lexeme[0], context.line, context.column);
    
    #ifdef DEBUG_LEXER
    printf("[DEBUG] Unknown token: '%c' (ASCII %d) at line %d, column %d\n", 
           context.lexeme[0], (int)context.lexeme[0], context.line, context.column);
    #endif
    
    return UNKNOWN_TOKEN;
}

/**
 * Utility function to get token name for debugging.
 */
const char* getTokenName(int token) {
    switch (token) {
        case OPEN_BRACES: return "OPEN_BRACES";
        case CLOSE_BRACES: return "CLOSE_BRACES";
        case OPEN_BRACKET: return "OPEN_BRACKET";
        case CLOSE_BRACKET: return "CLOSE_BRACKET";
        case OPEN_PAREN: return "OPEN_PAREN";
        case CLOSE_PAREN: return "CLOSE_PAREN";
        case COLON: return "COLON";
        case COMMA: return "COMMA";
        case METHODS: return "METHODS";
        case METHOD: return "METHOD";
        case PARAMS: return "PARAMS";
        case DESCRIPTION: return "DESCRIPTION";
        case TYPE: return "TYPE";
        case STYLE: return "STYLE";
        case TITLE: return "TITLE";
        case RANGE: return "RANGE";
        case REGEX: return "REGEX";
        case RELATED: return "RELATED";
        case TRUE: return "TRUE";
        case FALSE: return "FALSE";
        case NULL_VALUE: return "NULL_VALUE";
        case STRING: return "STRING";
        case INTEGER_NUMBER: return "INTEGER_NUMBER";
        case FLOAT_NUMBER: return "FLOAT_NUMBER";
        case IDENTIFIER: return "IDENTIFIER";
        case UNKNOWN_TOKEN: return "UNKNOWN_TOKEN";
        default: return "UNKNOWN";
    }
}

/**
 * Prints lexical analysis statistics.
 */
void printLexicalAnalysisStatistics() {
    printf("\n=== Lexical Analysis Statistics ===\n");
    printf("Total tokens processed: %d\n", totalTokens);
    printf("Total characters processed: %d\n", totalCharacters);
    printf("Total lines processed: %d\n", yylineno);
    if (commentDepth > 0) {
        printf("Warning: %d unclosed multiline comment(s)\n", commentDepth);
    }
    printf("===================================\n");
}

/**
 * Resets lexical analysis counters.
 */
void resetLexicalAnalysisCounters() {
    totalTokens = 0;
    totalCharacters = 0;
    commentDepth = 0;
    yylineno = 1;
}