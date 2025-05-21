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

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

void BeginStringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
void EndStringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token StringLexemeAction(LexicalAnalyzerContext *context, Token token);

void IgnoredLexemeAction(LexicalAnalyzerContext *context);
Token UnknownLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token KeywordLexemeAction(LexicalAnalyzerContext *context, Token token);

#endif // FLEX_ACTIONS_H