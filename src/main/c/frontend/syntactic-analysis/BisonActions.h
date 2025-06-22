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

Program *ProgramSemanticAction(CompilerState *compilerState, MethodTitle *methods, VariablesTitle *variables, StyleTitle *style);

MethodTitle *MethodTitleSemanticAction(Token title, MethodList *methods);
MethodList *MethodListSemanticAction(Method *method, MethodList *methods);
Method *MethodSemanticAction(char *name, MethodContent *content);
MethodContent *MethodContentSemanticAction(ParamsTitle *params, char *description, char *type, RelatedTitle *related, VariablesTitle *variables);

ParamsTitle *ParamsTitleSemanticAction(Token title, ParamsList *params);
ParamsList *ParamsListSemanticAction(Param *Param, ParamsList *Params);
Param *ParamSemanticAction(char *name, ParamData *data);
ParamData *ParamDataSemanticAction(char *type, char *regex, char *range, char *description);

RelatedTitle *RelatedTitleSemanticAction(Token title, RelatedList *related);
RelatedList *RelatedListSemanticAction(char *name, RelatedList *related);

VariablesTitle *VariablesTitleSemanticAction(Token title, VariableList *variables);
VariableList *VariableListSemanticAction(Variable *variable, VariableList *variables);
Variable *VariableSemanticAction(char *name, VariableData *data);
VariableData *VariableDataSemanticAction(char *type, char *description);

StyleTitle *StyleTitleSemanticAction(Token title, Style *method_style, Style *variable_style);
Style *StyleSemanticAction(StyleList *title, StyleList *description);
StyleList *StyleListSemanticAction(StyleStructure *style, StyleList *next);
StyleStructure *StyleStructureSemanticAction(char *label, char *value);

#endif
