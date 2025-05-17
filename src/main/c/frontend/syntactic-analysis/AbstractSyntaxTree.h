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

typedef struct Program Program;
typedef struct MethodList MethodList;
typedef struct Method Method;
typedef struct MethodContent MethodContent;
typedef struct ParamsList ParamsList;
typedef struct Param Param;
typedef struct ParamData ParamData;
typedef struct VariableList VariableList;
typedef struct Variable Variable;
typedef struct VariableData VariableData;
typedef struct RelatedList RelatedList;
typedef struct Style Style;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

struct Program
{
	MethodList *methods;
	Style *style;
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

struct MethodContent
{
	ParamsList *params;
	char *description;
	char *type;
	RelatedList *related;
	VariableList *variables;
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

struct RelatedList
{
	char *name;
	RelatedList *next;
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
