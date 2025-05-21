%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	Token token;
	char * string;

	/** Non-terminals. */
	Program * Program;

	MethodTitle * methods;
	MethodList * method_list;
 	Method * method;
	MethodContent * method_content;

	ParamsTitle * params;
	ParamsList * params_list;
	Param * param;
	ParamData * param_data;

	VariablesTitle * variables;
	VariableList * variable_list;
	Variable * variable;
	VariableData * variable_data;

	RelatedTitle * related;
	RelatedList * related_list;

	StyleTitle * style;
	Style * style_content;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
/* %destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor> */

/** Terminals. */
%token <token> METHODS
%token <token> STYLE
%token <token> PARAMS
%token <token> RANGE
%token <token> REGEX
%token <token> DESCRIPTION
%token <token> RELATED
%token <token> TYPE
%token <token> OPEN_BRACES
%token <token> CLOSE_BRACES
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> COLON
%token <token> COMMA
%token <token> TITLE
%token <token> VARIABLES
%token <string> STRING

%token <token> UNKNOWN

/** Non-terminals. */
%type <param_data> param_data
%type <variable_data> variable_data
%type <style_content> method_style_content
%type <style_content> variables_style_content

%type <Program> program

%type <methods> methods
%type <method_list> method_list
%type <method> method
%type <method_content> method_content

%type <params> params
%type <params_list> param_list
%type <param> param

%type <variables> variables
%type <variable_list> variable_list
%type <variable> variable

%type <related> related
%type <related_list> related_list

%type <style> style
/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
/* %left ADD SUB
%left MUL DIV */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program:
	OPEN_BRACES methods[met] COMMA variables[var] COMMA style[sty] CLOSE_BRACES		{$$ = ProgramSemanticAction(currentCompilerState(), $met, $var, $sty);}
	| OPEN_BRACES methods[met] COMMA style[sty] CLOSE_BRACES						{$$ = ProgramSemanticAction(currentCompilerState(), $met, NULL, $sty);}
	| OPEN_BRACES variables[var] COMMA style[sty] CLOSE_BRACES						{$$ = ProgramSemanticAction(currentCompilerState(), NULL, $var, $sty);}
	| OPEN_BRACES methods[met] COMMA variables[var] CLOSE_BRACES 					{$$ = ProgramSemanticAction(currentCompilerState(), $met, $var, NULL);}
	| OPEN_BRACES methods[met] CLOSE_BRACES											{$$ = ProgramSemanticAction(currentCompilerState(), $met, NULL, NULL);}
	| OPEN_BRACES variables[var] CLOSE_BRACES										{$$ = ProgramSemanticAction(currentCompilerState(), NULL, $var, NULL);}

	;

methods:
	METHODS COLON OPEN_BRACES method_list CLOSE_BRACES	{$$ = MethodTitleSemanticAction($1, $4);}
	;

method_list:
	%empty							{$$ = NULL;}	
	| method						{$$ = MethodListSemanticAction($1, NULL);}
	| method COMMA method_list		{$$ = MethodListSemanticAction($1, $3);}
	;

method:
	STRING COLON OPEN_BRACES method_content CLOSE_BRACES	{$$ = MethodSemanticAction($1, $4);}
	;

method_content:
	params COMMA DESCRIPTION COLON STRING[desc] COMMA TYPE COLON STRING[type] COMMA related[rel] COMMA variables[var]	{$$ = MethodContentSemanticAction($1, $desc, $type, $rel, $var);}
	| params COMMA DESCRIPTION COLON STRING[desc] COMMA TYPE COLON STRING[type] COMMA related[rel]	{$$ = MethodContentSemanticAction($1, $desc, $type, $rel, NULL);}
	| params COMMA DESCRIPTION COLON STRING[desc] COMMA TYPE COLON STRING[type] COMMA variables[var]	{$$ = MethodContentSemanticAction($1, $desc, $type, NULL, $var);}
	| params COMMA DESCRIPTION COLON STRING[desc] COMMA TYPE COLON STRING[type]	{$$ = MethodContentSemanticAction($1, $desc, $type, NULL, NULL);}
	;

params:
	PARAMS COLON OPEN_BRACES param_list CLOSE_BRACES	{$$ = ParamsTitleSemanticAction($1, $4);}
	;

param_list:
	%empty							{$$ = NULL;}
	| param							{$$ = ParamsListSemanticAction($1, NULL);}
	| param COMMA param_list		{$$ = ParamsListSemanticAction($1, $3);}
	;

param:
	STRING COLON OPEN_BRACES param_data CLOSE_BRACES		{$$ = ParamSemanticAction($1, $4);}
	;

param_data:
	TYPE COLON STRING[type] COMMA REGEX COLON STRING[regex] COMMA RANGE COLON STRING[range] COMMA DESCRIPTION COLON STRING[desc]			{$$ = ParamDataSemanticAction($type, $regex, $range, $desc);}
	| TYPE COLON STRING[type] COMMA RANGE COLON STRING[range] COMMA DESCRIPTION COLON STRING[desc]			{$$ = ParamDataSemanticAction($type, NULL, $range, $desc);}
	| TYPE COLON STRING[type] COMMA REGEX COLON STRING[regex] COMMA DESCRIPTION COLON STRING[desc]			{$$ = ParamDataSemanticAction($type, $regex, NULL, $desc);}
	| TYPE COLON STRING[type] COMMA DESCRIPTION COLON STRING[desc]			{$$ = ParamDataSemanticAction($type, NULL, NULL, $desc);}
	;

related:
	RELATED COLON OPEN_BRACKET related_list CLOSE_BRACKET	{$$ = RelatedTitleSemanticAction($1, $4);}
	;

related_list:
	%empty								{$$= NULL;}
	| STRING							{$$ = RelatedListSemanticAction($1, NULL);}
	| STRING COMMA related_list			{$$ = RelatedListSemanticAction($1, $3);}
	;

variables:
	VARIABLES COLON OPEN_BRACES variable_list CLOSE_BRACES 	{$$ = VariablesTitleSemanticAction($1, $4);}
	;	

variable_list:
	%empty								{$$ = NULL;}
	| variable							{$$ = VariableListSemanticAction($1, NULL);}
	| variable COMMA variable_list		{$$ = VariableListSemanticAction($1, $3);}
	;

variable:
	STRING COLON OPEN_BRACES variable_data CLOSE_BRACES {$$ = VariableSemanticAction($1, $4);}
	;

variable_data:
	TYPE COLON STRING[type] COMMA DESCRIPTION COLON STRING[desc]  { $$ = VariableDataSemanticAction($type, $desc); }
	;

style:
	STYLE COLON OPEN_BRACES method_style_content[met] COMMA variables_style_content[var] CLOSE_BRACES		{$$ = StyleTitleSemanticAction($1, $met, $var);}
	| STYLE COLON OPEN_BRACES method_style_content[met] CLOSE_BRACES										{$$ = StyleTitleSemanticAction($1, $met, NULL);}
	| STYLE COLON OPEN_BRACES variables_style_content[var] CLOSE_BRACES										{$$ = StyleTitleSemanticAction($1, NULL, $var);}
	;

method_style_content:
	METHODS COLON OPEN_BRACES TITLE COLON STRING[title] COMMA DESCRIPTION COLON STRING[desc] CLOSE_BRACES		{$$ = StyleSemanticAction($title,$desc);}
	| METHODS COLON  OPEN_BRACES TITLE COLON STRING[title] CLOSE_BRACES											{$$ = StyleSemanticAction($title,NULL);}
	| METHODS COLON  OPEN_BRACES DESCRIPTION COLON STRING[desc] CLOSE_BRACES									{$$ = StyleSemanticAction(NULL,$desc);}
	;

variables_style_content:
	VARIABLES COLON  OPEN_BRACES TITLE COLON STRING[title] COMMA DESCRIPTION COLON STRING[desc] CLOSE_BRACES		{$$ = StyleSemanticAction($title,$desc);}
	| VARIABLES COLON  OPEN_BRACES TITLE COLON STRING[title] CLOSE_BRACES											{$$ = StyleSemanticAction($title,NULL);}
	| VARIABLES COLON  OPEN_BRACES DESCRIPTION COLON STRING[desc] CLOSE_BRACES										{$$ = StyleSemanticAction(NULL,$desc);}
	;

%%
