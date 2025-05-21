#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeAbstractSyntaxTreeModule()
{
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

// void releaseConstant(Program *program)
// {
// 	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
// 	if (program != NULL)
// 	{
// 		free(program);
// 	}
// }

// void releaseExpression(Expression * expression) {
// 	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
// 	if (expression != NULL) {
// 		switch (expression->type) {
// 			case ADDITION:
// 			case DIVISION:
// 			case MULTIPLICATION:
// 			case SUBTRACTION:
// 				releaseExpression(expression->leftExpression);
// 				releaseExpression(expression->rightExpression);
// 				break;
// 			case FACTOR:
// 				releaseFactor(expression->factor);
// 				break;
// 		}
// 		free(expression);
// 	}
// }

// void releaseFactor(Factor * factor) {
// 	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
// 	if (factor != NULL) {
// 		switch (factor->type) {
// 			case CONSTANT:
// 				releaseConstant(factor->constant);
// 				break;
// 			case EXPRESSION:
// 				releaseExpression(factor->expression);
// 				break;
// 		}
// 		free(factor);
// 	}
// }

void releaseMethods(MethodTitle *methods);
void releaseMethodTitle(Titles *title);
void releaseMethodList(MethodList *methods);
void releaseMethod(Method *method);
void releaseMethodContent(MethodContent *content);
void releaseParamsTitle(Titles *title);
void releaseParamsList(ParamsList *params);
void releaseParam(Param *param);
void releaseParamData(ParamData *data);
void releaseRelatedTitle(Titles *title);
void releaseRelatedList(RelatedList *related);
void releaseVariablesTitle(Titles *title);
void releaseVariablesList(VariableList *variables);
void releaseVariable(Variable *variable);
void releaseVariableData(VariableData *data);
void releaseStyleTitle(Titles *title);
void releaseStyle(Style *style);

void releaseProgram(Program *program)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program == NULL)
		return;

	if (program->methods != NULL)
	{
		releaseMethodList(program->methods->methods);
		free(program->methods);
	}
	if (program->variables != NULL)
	{
		releaseVariablesList(program->variables->variables);
		free(program->variables);
	}
	if (program->style != NULL)
	{
		releaseStyle(program->style->method_style);
		releaseStyle(program->style->variable_style);
		free(program->style);
	}
	free(program);
}

void releaseMethodList(MethodList *methods)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (methods != NULL)
	{
		releaseMethod(methods->method);
		releaseMethodList(methods->next);
		free(methods);
	}
}

void releaseMethod(Method *method)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (method != NULL)
	{
		free(method->name);
		releaseMethodContent(method->content);
		free(method);
	}
}

void releaseMethodContent(MethodContent *content)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (content != NULL)
	{
		if (content->params != NULL)
		{
			releaseParamsList(content->params->params);
			free(content->params);
		}
		free(content->description);
		free(content->type);

		if (content->related != NULL)
		{
			releaseRelatedList(content->related->related);
			free(content->related);
		}

		if (content->variables != NULL)
		{
			releaseVariablesList(content->variables->variables);
			free(content->variables);
		}

		free(content);
	}
}

void releaseParamsList(ParamsList *params)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (params != NULL)
	{
		releaseParam(params->param);
		releaseParamsList(params->next);
		free(params);
	}
}

void releaseParam(Param *param)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (param != NULL)
	{
		free(param->name);
		releaseParamData(param->data);
		free(param);
	}
}

void releaseParamData(ParamData *data)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (data != NULL)
	{
		free(data->type);
		free(data->regex);
		free(data->range);
		free(data->description);
		free(data);
	}
}

void releaseRelatedList(RelatedList *related)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (related != NULL)
	{
		free(related->name);
		releaseRelatedList(related->next);
		free(related);
	}
}

void releaseVariablesList(VariableList *variables)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (variables != NULL)
	{
		releaseVariable(variables->variable);
		releaseVariablesList(variables->next);
		free(variables);
	}
}

void releaseVariable(Variable *variable)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (variable != NULL)
	{
		free(variable->name);
		releaseVariableData(variable->data);
		free(variable);
	}
}
void releaseVariableData(VariableData *data)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (data != NULL)
	{
		free(data->type);
		free(data->description);
		free(data);
	}
}

void releaseStyle(Style *style)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (style != NULL)
	{
		free(style->title);
		free(style->description);
		free(style);
	}
}
