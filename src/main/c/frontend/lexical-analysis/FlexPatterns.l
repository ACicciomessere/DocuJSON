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
DIGIT           [0-9]
LETTER          [a-zA-Z]
UNDERSCORE      [_]
IDENTIFIER      ({LETTER}|{UNDERSCORE})({LETTER}|{DIGIT}|{UNDERSCORE})*
INTEGER         {DIGIT}+
FLOAT           {DIGIT}+\.{DIGIT}+
STRING_CHAR     [^"\\]
ESCAPE_CHAR     \\[\"\\nrtbf/]
STRING_CONTENT  ({STRING_CHAR}|{ESCAPE_CHAR})*

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"//".*\n						    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"{"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
"}"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }
"["                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PAREN); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PAREN); }
":"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), COLON); }
","                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), QUOTE); }
"""                                 


"methods"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), METHODS); }
"method"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), METHOD); }
"params"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), PARAMS); }
"description"						{ return KeywordLexemeAction(createLexicalAnalyzerContext(), DESCRIPTION); }
"type"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"style"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"title"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), TITLE); }
"range"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), RANGE); }
"regex"							    { return KeywordLexemeAction(createLexicalAnalyzerContext(), REGEX); }
"related"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), RELATED); }


"true"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), TRUE); }
"false"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), FALSE); }
"null"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), NULL_VALUE); }


{FLOAT}                             { return NumberLexemeAction(createLexicalAnalyzerContext(), FLOAT_NUMBER); }
{INTEGER}                           { return NumberLexemeAction(createLexicalAnalyzerContext(), INTEGER_NUMBER); }


\"{STRING_CONTENT}\"                { return StringLexemeAction(createLexicalAnalyzerContext(), STRING); }


{IDENTIFIER}                        { return IdentifierLexemeAction(createLexicalAnalyzerContext(), IDENTIFIER); }


[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }



%%

#include "FlexExport.h"