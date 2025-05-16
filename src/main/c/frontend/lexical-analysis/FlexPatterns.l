%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"{"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
"}"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BARCES); }
"["                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
":"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), COLON); }
","                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), COMMA); }

"method"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), METHOD); }
"variables"						    { return KeywordLexemeAction(createLexicalAnalyzerContext(), VARIABLES); }
"style"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"params"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), PARAMS); }
"range"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), RANGE); }
"regex"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), REGEX); }
"description"						{ return KeywordLexemeAction(createLexicalAnalyzerContext(), DESCRIPTION); }
"related"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), RELATED); }
"type"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"title"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), TITLE); }

[[:string:]]+						{ return StringLexemeAction(createLexicalAnalyzerContext(), STRING); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
