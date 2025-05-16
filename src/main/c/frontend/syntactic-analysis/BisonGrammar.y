%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	Token token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>

/** Terminals. */
%token <integer> INTEGER
%token <token> METHOD
%token <token> VARIABLES
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
%token <token> STRING
%token <token> COMMA
%token <token> TITLE


%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression
	;

expression:  OPEN_BRACES METHOD COLON functions CLOSE_BRACES 
	| COMMA STYLE COLON OPEN_BRACES METHOD COLON OPEN_BRACES TITLE COLON STRING COMMA
	| DESCRIPTION COLON STRING
	| CLOSE_BRACES CLOSE_BRACES

functions: OPEN_BRACES STRING COLON params COMMA
	| DESCRIPTION COLON STRING COMMA	
	| TYPE COLON STRING
	| RELATED COLON OPEN_BRACKET STRING COMMA STRING CLOSE_BRACKET COMMA
	| variables 		
	| CLOSE_BRACES
	;

params: OPEN_BRACES PARAMS COLON parameters CLOSE_BRACES ;

parameters: OPEN_BRACES STRING COLON parameters_content CLOSE_BRACES ;

parameters_content: OPEN_BRACES TYPE COLON STRING COMMA
	| REGEX COLON STRING COMMA	
	| RANGE COLON STRING COMMA					
	;

variables: OPEN_BRACES STRING COLON variables_content CLOSE_BRACES ;

variables_content: OPEN_BRACES TYPE COLON STRING COMMA
	| DESCRIPTION COLON STRING
	| CLOSE_BRACES				
	;

%%
