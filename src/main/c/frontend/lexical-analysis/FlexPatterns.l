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
%x STRING_REGION

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

"\""											{ BEGIN(STRING_REGION); BeginStringLexemeAction(createLexicalAnalyzerContext()); }
<STRING_REGION>"\""									{ EndStringLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<STRING_REGION>[^"\n]*									{ return StringLexemeAction(createLexicalAnalyzerContext(), STRING); }




"{"									{ return KeywordLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
"}"									{ return KeywordLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }
"["                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
":"									{ return KeywordLexemeAction(createLexicalAnalyzerContext(), COLON); }
","                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), COMMA); }
"methods"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), METHODS); }
"params"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), PARAMS); }
"description"						{ return KeywordLexemeAction(createLexicalAnalyzerContext(), DESCRIPTION); }
"type"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"style"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"title"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), TITLE); }
"range"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), RANGE); }
"regex"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), REGEX); }
"related"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), RELATED); }
"variables"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), VARIABLES); }



[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"//".*\n						    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

.                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }



%%

#include "FlexExport.h"