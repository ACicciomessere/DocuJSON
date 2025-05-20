#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum Titles Titles;

typedef struct Program Program;

typedef struct MethodTitle MethodTitle;
typedef struct MethodList MethodList;
typedef struct Method Method;
typedef struct MethodContent MethodContent;

typedef struct ParamsTitle ParamsTitle;
typedef struct ParamsList ParamsList;
typedef struct Param Param;
typedef struct ParamData ParamData;

typedef struct VariablesTitle VariablesTitle;
typedef struct VariableList VariableList;
typedef struct Variable Variable;
typedef struct VariableData VariableData;

typedef struct RelatedTitle RelatedTitle;
typedef struct RelatedList RelatedList;

typedef struct StyleTitle StyleTitle;
typedef struct Style Style;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

struct Program
{
	MethodTitle *methods;
	StyleTitle *style;
};

struct MethodTitle
{
	Token title;
	MethodList *methods;
};

struct MethodList
{
	Method *method;
	MethodList *next;
};

struct Method
{
	char *name;
	MethodContent *content;
};

// struct MethodContent
// {
// 	ParamsList *params;
// 	char *description;
// 	char *type;
// 	RelatedList *related;
// 	VariableList *variables;
// };

struct MethodContent
{
	ParamsTitle *params;
	char *description;
	char *type;
	RelatedTitle *related;
	VariablesTitle *variables;
};

struct ParamsTitle
{
	Token title;
	ParamsList *params;
};

struct ParamsList
{
	Param *param;
	ParamsList *next;
};

struct Param
{
	char *name;
	ParamData *data;
};

struct ParamData
{
	char *type;
	char *regex;
	char *range;
	char *description;
};

struct RelatedTitle
{
	Token title;
	RelatedList *related;
};

struct RelatedList
{
	char *name;
	RelatedList *next;
};

struct VariablesTitle
{
	Token title;
	VariableList *variables;
};

struct VariableList
{
	Variable *variable;
	VariableList *next;
};

struct Variable
{
	char *name;
	VariableData *data;
};

struct VariableData
{
	char *type;
	char *description;
};

struct StyleTitle
{
	Token title;
	Style *style;
};

struct Style
{
	char *title;
	char *description;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program *program);

#endif
