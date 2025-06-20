#ifndef DOCU_JSON_HEADER
#define DOCU_JSON_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <string.h>
#include <ctype.h>

/** Initialize module's internal state. */
void initializeDocuJSONModule();

/** Shutdown module's internal state. */
void shutdownDocuJSONModule();

/**
 * The result of a validation operation. It's considered valid only if "succeed" is true.
 */
typedef struct
{
    boolean succeed;
    char *error_message;
    int error_count;
} ValidationResult;

/**
 * Validation configuration and rules
 */
typedef struct
{
    boolean require_method_description;
    boolean require_param_types;
    boolean require_param_descriptions;
    boolean validate_method_names;
    boolean validate_param_names;
    int max_method_name_length;
    int max_param_name_length;
    int min_description_length;
} ValidationConfig;

/**
 * Creates a default validation configuration
 */
ValidationConfig createDefaultValidationConfig();

/**
 * Validates the complete program structure
 */
ValidationResult validateProgram(Program *program, ValidationConfig *config);

/**
 * Validates all methods in the program
 */
ValidationResult validateMethods(MethodTitle *methods, ValidationConfig *config);

/**
 * Validates a single method
 */
ValidationResult validateMethod(Method *method, ValidationConfig *config);

/**
 * Validates method content (description, type, parameters, etc.)
 */
ValidationResult validateMethodContent(MethodContent *content, ValidationConfig *config);

/**
 * Validates method parameters
 */
ValidationResult validateParameters(ParamsTitle *params, ValidationConfig *config);

/**
 * Validates a single parameter
 */
ValidationResult validateParameter(Param *param, ValidationConfig *config);

/**
 * Validates parameter data (type, regex, range, description)
 */
ValidationResult validateParameterData(ParamData *data, ValidationConfig *config);

/**
 * Validates variables section
 */
ValidationResult validateVariables(VariablesTitle *variables, ValidationConfig *config);

/**
 * Validates a single variable
 */
ValidationResult validateVariable(Variable *variable, ValidationConfig *config);

/**
 * Validates variable data
 */
ValidationResult validateVariableData(VariableData *data, ValidationConfig *config);

/**
 * Validates related methods section
 */
ValidationResult validateRelated(RelatedTitle *related, ValidationConfig *config);

/**
 * Validates style configuration
 */
ValidationResult validateStyle(StyleTitle *style, ValidationConfig *config);

/**
 * Helper functions for specific validations
 */

/**
 * Validates that a string is a valid identifier (method/param name)
 */
boolean isValidIdentifier(const char *name);

/**
 * Validates that a string is a valid type specification
 */
boolean isValidType(const char *type);

/**
 * Validates that a regex pattern is syntactically correct
 */
boolean isValidRegex(const char *regex);

/**
 * Validates that a range specification is correct (e.g., "1-100", ">0", "<50")
 */
boolean isValidRange(const char *range);

/**
 * Validates that a description meets minimum requirements
 */
boolean isValidDescription(const char *description, int min_length);

/**
 * Creates an error message for validation failures
 */
char* createErrorMessage(const char *context, const char *error);

/**
 * Combines multiple validation results
 */
ValidationResult combineValidationResults(ValidationResult *results, int count);

/**
 * Releases memory used by ValidationResult
 */
void releaseValidationResult(ValidationResult *result);

#endif