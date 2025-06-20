#include "DocuJSON.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeDocuJSONModule()
{
    _logger = createLogger("DocuJSON");
}

void shutdownDocuJSONModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }
}

/** PRIVATE FUNCTIONS */

static ValidationResult _createValidResult();
static ValidationResult _createInvalidResult(const char *error_message);
static ValidationResult _validateMethodList(MethodList *methods, ValidationConfig *config);
static ValidationResult _validateParamsList(ParamsList *params, ValidationConfig *config);
static ValidationResult _validateVariablesList(VariableList *variables, ValidationConfig *config);
static ValidationResult _validateRelatedList(RelatedList *related, ValidationConfig *config);

/**
 * Creates a successful validation result
 */
static ValidationResult _createValidResult()
{
    ValidationResult result = {
        .succeed = true,
        .error_message = NULL,
        .error_count = 0
    };
    return result;
}

/**
 * Creates a failed validation result with error message
 */
static ValidationResult _createInvalidResult(const char *error_message)
{
    ValidationResult result = {
        .succeed = false,
        .error_message = malloc(strlen(error_message) + 1),
        .error_count = 1
    };
    strcpy(result.error_message, error_message);
    return result;
}

/**
 * Validates a list of methods recursively
 */
static ValidationResult _validateMethodList(MethodList *methods, ValidationConfig *config)
{
    if (methods == NULL)
    {
        return _createValidResult();
    }

    ValidationResult currentResult = validateMethod(methods->method, config);
    ValidationResult nextResult = _validateMethodList(methods->next, config);

    if (currentResult.succeed && nextResult.succeed)
    {
        releaseValidationResult(&nextResult);
        return currentResult;
    }
    else
    {
        ValidationResult results[] = {currentResult, nextResult};
        ValidationResult combined = combineValidationResults(results, 2);
        return combined;
    }
}

/**
 * Validates a list of parameters recursively
 */
static ValidationResult _validateParamsList(ParamsList *params, ValidationConfig *config)
{
    if (params == NULL)
    {
        return _createValidResult();
    }

    ValidationResult currentResult = validateParameter(params->param, config);
    ValidationResult nextResult = _validateParamsList(params->next, config);

    if (currentResult.succeed && nextResult.succeed)
    {
        releaseValidationResult(&nextResult);
        return currentResult;
    }
    else
    {
        ValidationResult results[] = {currentResult, nextResult};
        ValidationResult combined = combineValidationResults(results, 2);
        return combined;
    }
}

/**
 * Validates a list of variables recursively
 */
static ValidationResult _validateVariablesList(VariableList *variables, ValidationConfig *config)
{
    if (variables == NULL)
    {
        return _createValidResult();
    }

    ValidationResult currentResult = validateVariable(variables->variable, config);
    ValidationResult nextResult = _validateVariablesList(variables->next, config);

    if (currentResult.succeed && nextResult.succeed)
    {
        releaseValidationResult(&nextResult);
        return currentResult;
    }
    else
    {
        ValidationResult results[] = {currentResult, nextResult};
        ValidationResult combined = combineValidationResults(results, 2);
        return combined;
    }
}

/**
 * Validates a list of related methods recursively
 */
static ValidationResult _validateRelatedList(RelatedList *related, ValidationConfig *config)
{
    if (related == NULL)
    {
        return _createValidResult();
    }

    if (related->name == NULL || strlen(related->name) == 0)
    {
        return _createInvalidResult("Related method name cannot be empty");
    }

    if (!isValidIdentifier(related->name))
    {
        char *error = createErrorMessage("Related method", "Invalid identifier format");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    return _validateRelatedList(related->next, config);
}

/** PUBLIC FUNCTIONS */

ValidationConfig createDefaultValidationConfig()
{
    ValidationConfig config = {
        .require_method_description = true,
        .require_param_types = true,
        .require_param_descriptions = false,
        .validate_method_names = true,
        .validate_param_names = true,
        .max_method_name_length = 50,
        .max_param_name_length = 30,
        .min_description_length = 5
    };
    return config;
}

ValidationResult validateProgram(Program *program, ValidationConfig *config)
{
    if (program == NULL)
    {
        logError(_logger, "Program is NULL");
        return _createInvalidResult("Program structure is NULL");
    }

    ValidationResult methodsResult = validateMethods(program->methods, config);
    ValidationResult variablesResult = validateVariables(program->variables, config);
    ValidationResult styleResult = validateStyle(program->style, config);

    ValidationResult results[] = {methodsResult, variablesResult, styleResult};
    return combineValidationResults(results, 3);
}

ValidationResult validateMethods(MethodTitle *methods, ValidationConfig *config)
{
    if (methods == NULL)
    {
        logWarning(_logger, "No methods section found");
        return _createValidResult(); // Methods are optional
    }

    if (methods->methods == NULL)
    {
        logWarning(_logger, "Methods list is empty");
        return _createValidResult(); // Empty methods list is also valid
    }

    return _validateMethodList(methods->methods, config);
}

ValidationResult validateMethod(Method *method, ValidationConfig *config)
{
    if (method == NULL)
    {
        return _createInvalidResult("Method is NULL");
    }

    if (method->name == NULL || strlen(method->name) == 0)
    {
        return _createInvalidResult("Method name cannot be empty");
    }

    if (config->validate_method_names && !isValidIdentifier(method->name))
    {
        char *error = createErrorMessage("Method name", "Invalid identifier format");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    if (config->max_method_name_length > 0 && strlen(method->name) > config->max_method_name_length)
    {
        char *error = createErrorMessage("Method name", "Exceeds maximum length");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    return validateMethodContent(method->content, config);
}

ValidationResult validateMethodContent(MethodContent *content, ValidationConfig *config)
{
    if (content == NULL)
    {
        return _createInvalidResult("Method content is NULL");
    }

    if (config->require_method_description && 
        (content->description == NULL || !isValidDescription(content->description, config->min_description_length)))
    {
        return _createInvalidResult("Method description is required and must meet minimum length");
    }

    if (content->type == NULL || !isValidType(content->type))
    {
        return _createInvalidResult("Method type is required and must be valid");
    }

    ValidationResult paramsResult = validateParameters(content->params, config);
    ValidationResult variablesResult = validateVariables(content->variables, config);
    ValidationResult relatedResult = validateRelated(content->related, config);

    ValidationResult results[] = {paramsResult, variablesResult, relatedResult};
    return combineValidationResults(results, 3);
}

ValidationResult validateParameters(ParamsTitle *params, ValidationConfig *config)
{
    if (params == NULL)
    {
        return _createValidResult(); // Parameters are optional
    }

    return _validateParamsList(params->params, config);
}

ValidationResult validateParameter(Param *param, ValidationConfig *config)
{
    if (param == NULL)
    {
        return _createInvalidResult("Parameter is NULL");
    }

    if (param->name == NULL || strlen(param->name) == 0)
    {
        return _createInvalidResult("Parameter name cannot be empty");
    }

    if (config->validate_param_names && !isValidIdentifier(param->name))
    {
        char *error = createErrorMessage("Parameter name", "Invalid identifier format");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    if (config->max_param_name_length > 0 && strlen(param->name) > config->max_param_name_length)
    {
        char *error = createErrorMessage("Parameter name", "Exceeds maximum length");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    return validateParameterData(param->data, config);
}

ValidationResult validateParameterData(ParamData *data, ValidationConfig *config)
{
    if (data == NULL)
    {
        return _createInvalidResult("Parameter data is NULL");
    }

    if (config->require_param_types && (data->type == NULL || !isValidType(data->type)))
    {
        return _createInvalidResult("Parameter type is required and must be valid");
    }

    if (data->regex != NULL && !isValidRegex(data->regex))
    {
        return _createInvalidResult("Parameter regex pattern is invalid");
    }

    if (data->range != NULL && !isValidRange(data->range))
    {
        return _createInvalidResult("Parameter range specification is invalid");
    }

    if (config->require_param_descriptions && 
        (data->description == NULL || !isValidDescription(data->description, config->min_description_length)))
    {
        return _createInvalidResult("Parameter description is required and must meet minimum length");
    }
    else if (data->description != NULL && !isValidDescription(data->description, config->min_description_length))
    {
        return _createInvalidResult("Parameter description must meet minimum length if provided");
    }

    return _createValidResult();
}

ValidationResult validateVariables(VariablesTitle *variables, ValidationConfig *config)
{
    if (variables == NULL)
    {
        return _createValidResult(); // Variables are optional
    }

    return _validateVariablesList(variables->variables, config);
}

ValidationResult validateVariable(Variable *variable, ValidationConfig *config)
{
    if (variable == NULL)
    {
        return _createInvalidResult("Variable is NULL");
    }

    if (variable->name == NULL || strlen(variable->name) == 0)
    {
        return _createInvalidResult("Variable name cannot be empty");
    }

    if (!isValidIdentifier(variable->name))
    {
        char *error = createErrorMessage("Variable name", "Invalid identifier format");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    return validateVariableData(variable->data, config);
}

ValidationResult validateVariableData(VariableData *data, ValidationConfig *config)
{
    if (data == NULL)
    {
        return _createInvalidResult("Variable data is NULL");
    }

    if (data->type == NULL || !isValidType(data->type))
    {
        return _createInvalidResult("Variable type is required and must be valid");
    }

    if (data->description != NULL && !isValidDescription(data->description, config->min_description_length))
    {
        return _createInvalidResult("Variable description must meet minimum length if provided");
    }

    return _createValidResult();
}

ValidationResult validateRelated(RelatedTitle *related, ValidationConfig *config)
{
    if (related == NULL)
    {
        return _createValidResult(); // Related methods are optional
    }

    return _validateRelatedList(related->related, config);
}

ValidationResult validateStyle(StyleTitle *style, ValidationConfig *config)
{
    if (style == NULL)
    {
        return _createValidResult(); // Style is optional
    }

    // Basic style validation - can be extended
    if (style->method_style != NULL)
    {
        if (style->method_style->title == NULL)
        {
            return _createInvalidResult("Method style title cannot be NULL");
        }
    }

    if (style->variable_style != NULL)
    {
        if (style->variable_style->title == NULL)
        {
            return _createInvalidResult("Variable style title cannot be NULL");
        }
    }

    return _createValidResult();
}

/** HELPER FUNCTIONS */

boolean isValidIdentifier(const char *name)
{
    if (name == NULL || strlen(name) == 0)
    {
        return false;
    }

    // Must start with letter or underscore
    if (!isalpha(name[0]) && name[0] != '_')
    {
        return false;
    }

    // Rest can be letters, digits, or underscores
    for (int i = 1; name[i] != '\0'; i++)
    {
        if (!isalnum(name[i]) && name[i] != '_')
        {
            return false;
        }
    }

    return true;
}

boolean isValidType(const char *type)
{
    if (type == NULL || strlen(type) == 0)
    {
        return false;
    }

    // Basic type validation - extend as needed
    const char *valid_types[] = {
        "string", "int", "float", "boolean", "object", "array", "number", "void", "Date"
    };

    int num_types = sizeof(valid_types) / sizeof(valid_types[0]);
    for (int i = 0; i < num_types; i++)
    {
        if (strcmp(type, valid_types[i]) == 0)
        {
            return true;
        }
    }

    // Permitir tipos personalizados que no estén en la lista predefinida
    // pero que sean identificadores válidos
    return isValidIdentifier(type);
}

boolean isValidRegex(const char *regex)
{
    if (regex == NULL || strlen(regex) == 0)
    {
        return false;
    }

    // Basic regex validation - check for balanced brackets and basic syntax
    int bracket_count = 0;
    int paren_count = 0;

    for (int i = 0; regex[i] != '\0'; i++)
    {
        switch (regex[i])
        {
        case '[':
            bracket_count++;
            break;
        case ']':
            bracket_count--;
            if (bracket_count < 0) return false;
            break;
        case '(':
            paren_count++;
            break;
        case ')':
            paren_count--;
            if (paren_count < 0) return false;
            break;
        }
    }

    return bracket_count == 0 && paren_count == 0;
}

boolean isValidRange(const char *range)
{
    if (range == NULL || strlen(range) == 0)
    {
        return false;
    }

    // Basic range validation: ">10", "<100", "1-50", ">=0", "<=999", "[0,100]"
    if (range[0] == '>' || range[0] == '<')
    {
        int start = (range[1] == '=') ? 2 : 1;
        for (int i = start; range[i] != '\0'; i++)
        {
            if (!isdigit(range[i]) && range[i] != '.' && range[i] != '-')
            {
                return false;
            }
        }
        return true;
    }

    // Range format "min-max"
    char *dash = strchr(range, '-');
    if (dash != NULL)
    {
        // Check if both parts are numbers
        for (char *p = (char*)range; p < dash; p++)
        {
            if (!isdigit(*p) && *p != '.')
            {
                return false;
            }
        }
        for (char *p = dash + 1; *p != '\0'; p++)
        {
            if (!isdigit(*p) && *p != '.')
            {
                return false;
            }
        }
        return true;
    }

    // Range format "[min,max]" - formato de array
    if (range[0] == '[' && range[strlen(range) - 1] == ']')
    {
        // Extraer el contenido entre corchetes
        char *content = malloc(strlen(range) - 1);
        strncpy(content, range + 1, strlen(range) - 2);
        content[strlen(range) - 2] = '\0';
        
        // Buscar la coma
        char *comma = strchr(content, ',');
        if (comma != NULL)
        {
            // Verificar que ambas partes sean números
            boolean valid = true;
            for (char *p = content; p < comma; p++)
            {
                if (!isdigit(*p) && *p != '.' && *p != ' ')
                {
                    valid = false;
                    break;
                }
            }
            for (char *p = comma + 1; *p != '\0'; p++)
            {
                if (!isdigit(*p) && *p != '.' && *p != ' ')
                {
                    valid = false;
                    break;
                }
            }
            free(content);
            return valid;
        }
        free(content);
    }

    return false;
}

boolean isValidDescription(const char *description, int min_length)
{
    if (description == NULL)
    {
        return false;
    }

    int length = strlen(description);
    if (length < min_length)
    {
        return false;
    }

    // Check if description has meaningful content (not just whitespace)
    boolean has_content = false;
    for (int i = 0; i < length; i++)
    {
        if (!isspace(description[i]))
        {
            has_content = true;
            break;
        }
    }

    return has_content;
}

char* createErrorMessage(const char *context, const char *error)
{
    size_t context_len = strlen(context);
    size_t error_len = strlen(error);
    size_t total_len = context_len + error_len + 4; // ": " + " - " + null terminator

    char *message = malloc(total_len);
    snprintf(message, total_len, "%s: %s", context, error);
    return message;
}

ValidationResult combineValidationResults(ValidationResult *results, int count)
{
    ValidationResult combined = _createValidResult();
    size_t total_error_length = 0;
    int total_errors = 0;

    // Calculate total error message length and count
    for (int i = 0; i < count; i++)
    {
        if (!results[i].succeed)
        {
            combined.succeed = false;
            total_errors += results[i].error_count;
            if (results[i].error_message != NULL)
            {
                total_error_length += strlen(results[i].error_message) + 2; // +2 for "; "
            }
        }
    }

    if (!combined.succeed)
    {
        combined.error_count = total_errors;
        combined.error_message = malloc(total_error_length + 1);
        combined.error_message[0] = '\0';

        // Concatenate all error messages
        for (int i = 0; i < count; i++)
        {
            if (!results[i].succeed && results[i].error_message != NULL)
            {
                if (strlen(combined.error_message) > 0)
                {
                    strcat(combined.error_message, "; ");
                }
                strcat(combined.error_message, results[i].error_message);
            }
        }
    }

    // Clean up individual results - only if they have error messages
    for (int i = 0; i < count; i++)
    {
        if (results[i].error_message != NULL)
        {
            releaseValidationResult(&results[i]);
        }
    }

    return combined;
}

void releaseValidationResult(ValidationResult *result)
{
    if (result != NULL && result->error_message != NULL)
    {
        free(result->error_message);
        result->error_message = NULL;
    }
}