#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeBisonActionsModule()
{
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char *functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char *functionName)
{
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */
Program *ProgramSemanticAction(CompilerState *compilerState, MethodTitle *methods, VariablesTitle *variables, StyleTitle *style)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program *program = calloc(1, sizeof(Program));
	program->methods = methods;
	program->variables = variables;
	program->style = style;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext())
	{
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else
	{
		compilerState->succeed = true;
	}
	return program;
}

char *echoString(char *string)
{
	return string;
}

ParamsList *ParamsListSemanticAction(Param *param, ParamsList *params)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParamsList *paramsList = calloc(1, sizeof(ParamsList));
	paramsList->param = param;
	paramsList->next = params;
	return paramsList;
}

Param *ParamSemanticAction(char *name, ParamData *data)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Param *param = calloc(1, sizeof(Param));
	param->name = name;
	param->data = data;
	return param;
}

ParamData *ParamDataSemanticAction(char *type, char *regex, char *range, char *description)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParamData *paramData = calloc(1, sizeof(ParamData));
	paramData->type = type;
	paramData->regex = regex;
	paramData->range = range;
	paramData->description = description;
	return paramData;
}

VariableList *VariableListSemanticAction(Variable *variable, VariableList *variables)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	VariableList *variableList = calloc(1, sizeof(VariableList));
	variableList->variable = variable;
	variableList->next = variables;
	return variableList;
}

Variable *VariableSemanticAction(char *name, VariableData *data)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Variable *variable = calloc(1, sizeof(Variable));
	variable->name = name;
	variable->data = data;
	return variable;
}

VariableData *VariableDataSemanticAction(char *type, char *description)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	VariableData *variableData = calloc(1, sizeof(VariableData));
	variableData->type = type;
	variableData->description = description;
	return variableData;
}

MethodContent *MethodContentSemanticAction(ParamsTitle *params, char *description, char *type, RelatedTitle *related, VariablesTitle *variables)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodContent *methodContent = calloc(1, sizeof(MethodContent));
	methodContent->params = params;
	methodContent->description = description;
	methodContent->type = type;
	methodContent->related = related;
	methodContent->variables = variables;
	return methodContent;
}

Method *MethodSemanticAction(char *name, MethodContent *content)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Method *method = calloc(1, sizeof(Method));
	method->name = name;
	method->content = content;
	return method;
}

MethodList *MethodListSemanticAction(Method *method, MethodList *methods)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodList *methodList = calloc(1, sizeof(MethodList));
	methodList->method = method;
	methodList->next = methods;
	return methodList;
}

RelatedList *RelatedListSemanticAction(char *name, RelatedList *related)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RelatedList *relatedList = calloc(1, sizeof(RelatedList));
	relatedList->name = name;
	relatedList->next = related;
	return relatedList;
}

Style *StyleSemanticAction(StyleList *title, StyleList *description)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Style *style = calloc(1, sizeof(Style));
	style->title = title;
	style->description = description;
	return style;
}

MethodTitle *MethodTitleSemanticAction(Token title, MethodList *methods)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MethodTitle *methodTitle = calloc(1, sizeof(MethodTitle));
	methodTitle->title = title;
	methodTitle->methods = methods;
	return methodTitle;
}

ParamsTitle *ParamsTitleSemanticAction(Token title, ParamsList *params)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParamsTitle *paramsTitle = calloc(1, sizeof(ParamsTitle));
	paramsTitle->title = title;
	paramsTitle->params = params;
	return paramsTitle;
}

VariablesTitle *VariablesTitleSemanticAction(Token title, VariableList *variables)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	VariablesTitle *variablesTitle = calloc(1, sizeof(VariablesTitle));
	variablesTitle->title = title;
	variablesTitle->variables = variables;
	return variablesTitle;
}

RelatedTitle *RelatedTitleSemanticAction(Token title, RelatedList *related)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	RelatedTitle *relatedTitle = calloc(1, sizeof(RelatedTitle));
	relatedTitle->title = title;
	relatedTitle->related = related;
	return relatedTitle;
}

StyleTitle *StyleTitleSemanticAction(Token title, Style *method_style, Style *variable_style)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleTitle *styleTitle = calloc(1, sizeof(StyleTitle));
	styleTitle->title = title;
	styleTitle->method_style = method_style;
	styleTitle->variable_style = variable_style;
	return styleTitle;
}

StyleList *StyleListSemanticAction(StyleStructure *style, StyleList *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleList *styleList = calloc(1, sizeof(StyleList));
	styleList->style = style;
	styleList->next = next;
	return styleList;
}

StyleStructure *StyleStructureSemanticAction(char *label, char *value)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleStructure *style = calloc(1, sizeof(StyleStructure));
	style->label = label;
	style->value = value;
	return style;
}