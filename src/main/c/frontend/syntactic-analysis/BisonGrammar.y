%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	Token token;
	char * string;
	ParamData * param_data;
	VariableData * variable_data;
	Style * style_content;

	/** Non-terminals. */
	Program * Program;

	MethodTitle * methods;
	MethodList * method_list;
 	Method * method;
	MethodContent * method_content;

	ParamsTitle * params;
	ParamsList * params_list;
	Param * param;

	VariablesTitle * variables;
	VariableList * variable_list;
	Variable * variable;

	RelatedTitle * related;
	RelatedList * related_list;

	StyleTitle * style;
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
%token <token> METHOD
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

%token <token> OPEN_PAREN CLOSE_PAREN
%token <token> TRUE FALSE NULL_VALUE
%token <token> QUOTE
%token <token> METHODS 
/* ??????????????????????????????????????*/
%token <string> STRING

/* ?????????????????????????????' ' */
%type <param_data> param_data
%type <variable_data> variable_data
%type <string> description
%type <string> type
%type <string> regex
%type <string> range
%type <string> title
%type <style_content> style_content

/** Non-terminals. */
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
	OPEN_BRACES methods COMMA style CLOSE_BRACES		{$$ = ProgramSemanticAction(currentCompilerState(), $2, $4);}
	;

methods:
	METHOD COLON OPEN_BRACES method_list CLOSE_BRACES	{$$ = MethodTitleSemanticAction($1, $4);}
	;

method_list:
	method							{$$ = MethodListSemanticAction($1, NULL);}
	| method COMMA method_list		{$$ = MethodListSemanticAction($1, $3);}
	;

method:
	STRING COLON OPEN_BRACES method_content CLOSE_BRACES	{$$ = MethodSemanticAction($1, $4);}
	;

method_content:
	params COMMA description COMMA type COMMA related COMMA variables	{$$ = MethodContentSemanticAction($1, $3, $5, $7, $9);}
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
	type COMMA regex COMMA range COMMA description			{$$ = ParamDataSemanticAction($1, $3, $5, $7);}
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
	type COMMA description  { $$ = VariableDataSemanticAction($1, $3); }
	;

style:
	STYLE COLON OPEN_BRACES style_content CLOSE_BRACES		{$$ = StyleTitleSemanticAction($1, $4);}
	;

style_content:
	METHOD COLON title COMMA description		{$$ = StyleSemanticAction($3,$5 );}
	;

/* name: value*/

description:
	DESCRIPTION COLON STRING		{$$ = echoString($3);}
	;	

type:
	TYPE COLON STRING			{$$ = echoString($3);}	
	;

regex:
	REGEX COLON STRING			{$$ = echoString($3);}
	;

range:
	/* RANGE COLON OPEN_BRACKET STRING COMMA STRING CLOSE_BRACKET */
	RANGE COLON STRING 			{$$ = echoString($3);}
	;

title:
	TITLE COLON STRING			{$$ = echoString($3);}	
	;


%%
