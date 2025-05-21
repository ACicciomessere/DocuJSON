#include "FlexActions.h"
#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Logger *_logger = NULL;
static boolean _logIgnoredLexemes = true;

void initializeFlexActionsModule()
{
    _logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
    _logger = createLogger("FlexActions");
}

void shutdownFlexActionsModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }
}

static void _logLexicalAnalyzerContext(const char *functionName, LexicalAnalyzerContext *lexicalAnalyzerContext);

/**
 * Logs a lexical-analyzer context in DEBUGGING level.
 */
static void _logLexicalAnalyzerContext(const char *functionName, LexicalAnalyzerContext *lexicalAnalyzerContext)
{
    char *escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
    logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)",
                 functionName,
                 escapedLexeme,
                 lexicalAnalyzerContext->currentContext,
                 lexicalAnalyzerContext->length,
                 lexicalAnalyzerContext->line);
    free(escapedLexeme);
}

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext)
{
    if (_logIgnoredLexemes)
    {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void EndMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext)
{
    if (_logIgnoredLexemes)
    {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void IgnoredLexemeAction(LexicalAnalyzerContext *context)
{
    if (_logIgnoredLexemes)
    {
        _logLexicalAnalyzerContext(__FUNCTION__, context);
    }
    destroyLexicalAnalyzerContext(context);
}

Token KeywordLexemeAction(LexicalAnalyzerContext *context, Token token)
{
    _logLexicalAnalyzerContext(__FUNCTION__, context);
    context->semanticValue->token = token;
    destroyLexicalAnalyzerContext(context);
    return token;
}

void BeginStringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext)
{
    if (_logIgnoredLexemes)
    {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void EndStringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext)
{
    if (_logIgnoredLexemes)
    {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

Token StringLexemeAction(LexicalAnalyzerContext *context, Token token)
{
    _logLexicalAnalyzerContext(__FUNCTION__, context);
    context->semanticValue->string = strdup(context->lexeme);
    destroyLexicalAnalyzerContext(context);
    return token;
}

Token UnknownLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext)
{
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return UNKNOWN;
}