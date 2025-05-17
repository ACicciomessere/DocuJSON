#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

Program *ProgramSemanticAction(CompilerState *compilerState, MethodList *methods, Style *style);
char *echoString(char *string);

MethodList *MethodListSemanticAction(Method *method, MethodList *methods);
Method *MethodSemanticAction(char *name, MethodContent *content);
MethodContent *MethodContentSemanticAction(ParamsList *params, char *description, char *type, RelatedList *related, VariableList *variables);

ParamsList *ParamsListSemanticAction(Param *Param, ParamsList *Params);
Param *ParamSemanticAction(char *name, ParamData *data);
ParamData *ParamDataSemanticAction(char *type, char *regex, char *range, char *description);

RelatedList *RelatedListSemanticAction(char *name, RelatedList *related);

VariableList *VariableListSemanticAction(Variable *variable, VariableList *variables);
Variable *VariableSemanticAction(char *name, VariableData *data);
VariableData *VariableDataSemanticAction(char *type, char *description);

Style *StyleSemanticAction(char *name, char *value);

#endif
