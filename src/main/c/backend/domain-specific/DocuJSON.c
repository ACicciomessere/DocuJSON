#include "DocuJSON.h"
#include "utils.h"

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
static ValidationResult _validateMethodDuplicates(MethodList *methods, ValidationConfig *config);
static boolean _areMethodsDuplicate(Method *method1, Method *method2);
static boolean _areParamsListsEqual(ParamsList *params1, ParamsList *params2);
static ValidationResult _validateRelatedFunctionsExist(RelatedList *related, MethodList *allMethods);
static boolean _methodExists(const char *methodName, MethodList *methods);
static ValidationResult _validateAllRelatedFunctions(MethodList *methods, ValidationConfig *config);
static ValidationResult _validateStyleList(StyleList *styleList, ValidationConfig *config);
static ValidationResult _validateStyleStructure(StyleStructure *style, ValidationConfig *config);

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

/**
 * Validates a list of method duplicates
 */
static ValidationResult _validateMethodDuplicates(MethodList *methods, ValidationConfig *config)
{
    if (methods == NULL)
    {
        return _createValidResult();
    }

    MethodList *current = methods;
    while (current != NULL)
    {
        MethodList *next = current->next;
        while (next != NULL)
        {
            if (_areMethodsDuplicate(current->method, next->method))
            {
                char error_message[512];
                snprintf(error_message, sizeof(error_message), 
                        "Duplicate method found: '%s' with identical name and parameters", 
                        current->method->name);
                return _createInvalidResult(error_message);
            }
            next = next->next;
        }
        current = current->next;
    }

    return _createValidResult();
}

/**
 * Checks if two methods are duplicates
 */
static boolean _areMethodsDuplicate(Method *method1, Method *method2)
{
    if (method1 == NULL || method2 == NULL)
    {
        return false;
    }

    // Check if method names are the same
    if (strcmp(method1->name, method2->name) != 0)
    {
        return false;
    }

    // If names are the same, check if parameters are also the same
    ParamsList *params1 = NULL;
    ParamsList *params2 = NULL;
    
    if (method1->content != NULL && method1->content->params != NULL)
    {
        params1 = method1->content->params->params;
    }
    
    if (method2->content != NULL && method2->content->params != NULL)
    {
        params2 = method2->content->params->params;
    }
    
    return _areParamsListsEqual(params1, params2);
}

/**
 * Checks if two parameter lists are equal
 */
static boolean _areParamsListsEqual(ParamsList *params1, ParamsList *params2)
{
    if (params1 == NULL && params2 == NULL)
    {
        return true;
    }
    if (params1 == NULL || params2 == NULL)
    {
        return false;
    }

    while (params1 != NULL && params2 != NULL)
    {
        // Check if param structures are valid
        if (params1->param == NULL || params2->param == NULL)
        {
            return false;
        }
        
        // Compare parameter names
        if (params1->param->name == NULL || params2->param->name == NULL)
        {
            return false;
        }
        
        if (strcmp(params1->param->name, params2->param->name) != 0)
        {
            return false;
        }
        
        // Compare parameter types
        char *type1 = NULL;
        char *type2 = NULL;
        
        if (params1->param->data != NULL)
        {
            type1 = params1->param->data->type;
        }
        
        if (params2->param->data != NULL)
        {
            type2 = params2->param->data->type;
        }
        
        if ((type1 == NULL && type2 != NULL) || (type1 != NULL && type2 == NULL))
        {
            return false;
        }
        
        if (type1 != NULL && type2 != NULL && strcmp(type1, type2) != 0)
        {
            return false;
        }
        
        params1 = params1->next;
        params2 = params2->next;
    }

    return params1 == NULL && params2 == NULL;
}

/**
 * Validates a list of related functions exist
 */
static ValidationResult _validateRelatedFunctionsExist(RelatedList *related, MethodList *allMethods)
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

    if (!_methodExists(related->name, allMethods))
    {
        char *error = createErrorMessage("Related method", "Method does not exist");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    return _validateRelatedFunctionsExist(related->next, allMethods);
}

/**
 * Checks if a method exists in a list of methods
 */
static boolean _methodExists(const char *methodName, MethodList *methods)
{
    if (methods == NULL)
    {
        return false;
    }

    MethodList *current = methods;
    while (current != NULL)
    {
        if (current->method != NULL && strcmp(current->method->name, methodName) == 0)
        {
            return true;
        }
        current = current->next;
    }

    return false;
}

/**
 * Validates all related functions for a list of methods
 */
static ValidationResult _validateAllRelatedFunctions(MethodList *methods, ValidationConfig *config)
{
    if (methods == NULL)
    {
        return _createValidResult();
    }

    MethodList *current = methods;
    ValidationResult combinedResult = _createValidResult();
    
    while (current != NULL)
    {
        if (current->method != NULL && current->method->content != NULL && current->method->content->related != NULL)
        {
            ValidationResult relatedFunctionsResult = _validateRelatedFunctionsExist(current->method->content->related->related, methods);
            if (!relatedFunctionsResult.succeed)
            {
                if (combinedResult.succeed)
                {
                    // First error, initialize the combined result
                    combinedResult = relatedFunctionsResult;
                }
                else
                {
                    // Additional error, combine with existing
                    ValidationResult results[] = {combinedResult, relatedFunctionsResult};
                    combinedResult = combineValidationResults(results, 2);
                }
            }
            else
            {
                releaseValidationResult(&relatedFunctionsResult);
            }
        }
        current = current->next;
    }

    return combinedResult;
}

/**
 * Validates a list of style structures recursively
 */
static ValidationResult _validateStyleList(StyleList *styleList, ValidationConfig *config)
{
    if (styleList == NULL)
    {
        return _createValidResult();
    }

    ValidationResult currentResult = _validateStyleStructure(styleList->style, config);
    ValidationResult nextResult = _validateStyleList(styleList->next, config);

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
 * Validates a single style structure
 */
static ValidationResult _validateStyleStructure(StyleStructure *style, ValidationConfig *config)
{
    if (style == NULL)
    {
        return _createInvalidResult("Style structure is NULL");
    }

    if (style->label == NULL || strlen(style->label) == 0)
    {
        return _createInvalidResult("Style property name cannot be empty");
    }

    if (style->value == NULL || strlen(style->value) == 0)
    {
        return _createInvalidResult("Style property value cannot be empty");
    }

    // Validar que la propiedad CSS sea válida
    if (!is_valid_property(style->label))
    {
        char *error = createErrorMessage("CSS property", "Invalid property name");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    // Validar que el valor CSS sea válido
    if (!is_valid_value(style->value))
    {
        char *error = createErrorMessage("CSS value", "Invalid property value");
        ValidationResult result = _createInvalidResult(error);
        free(error);
        return result;
    }

    return _createValidResult();
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

    ValidationResult duplicatesResult = _validateMethodDuplicates(methods->methods, config);
    ValidationResult methodListResult = _validateMethodList(methods->methods, config);
    ValidationResult relatedFunctionsResult = _validateAllRelatedFunctions(methods->methods, config);

    ValidationResult results[] = {duplicatesResult, methodListResult, relatedFunctionsResult};
    return combineValidationResults(results, 3);
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

    // Necesitamos acceder a la lista completa de métodos para validar que las funciones relacionadas existan
    // Esto se hará desde el nivel superior (validateMethods)
    return _validateRelatedList(related->related, config);
}

ValidationResult validateStyle(StyleTitle *style, ValidationConfig *config)
{
    if (style == NULL)
    {
        return _createValidResult(); // Style is optional
    }

    // Si se incluye un objeto style, debe tener al menos method_style o variable_style
    if (style->method_style == NULL && style->variable_style == NULL)
    {
        return _createInvalidResult("Style object cannot be empty - must contain method_style or variable_style");
    }

    ValidationResult methodStyleResult = _createValidResult();
    ValidationResult variableStyleResult = _createValidResult();

    // Validar method_style si existe
    if (style->method_style != NULL)
    {
        if (style->method_style->title == NULL)
        {
            methodStyleResult = _createInvalidResult("Method style title cannot be NULL");
        }
        else
        {
            ValidationResult titleResult = _validateStyleList(style->method_style->title, config);
            if (!titleResult.succeed)
            {
                methodStyleResult = _createInvalidResult("Method style title contains invalid CSS");
                releaseValidationResult(&titleResult);
            }
            else
            {
                releaseValidationResult(&titleResult);
            }
        }

        if (style->method_style->description != NULL)
        {
            ValidationResult descResult = _validateStyleList(style->method_style->description, config);
            if (!descResult.succeed)
            {
                if (methodStyleResult.succeed)
                {
                    methodStyleResult = _createInvalidResult("Method style description contains invalid CSS");
                }
                else
                {
                    // Combinar con el error existente
                    ValidationResult results[] = {methodStyleResult, _createInvalidResult("Method style description contains invalid CSS")};
                    methodStyleResult = combineValidationResults(results, 2);
                }
            }
            releaseValidationResult(&descResult);
        }
    }

    // Validar variable_style si existe
    if (style->variable_style != NULL)
    {
        if (style->variable_style->title == NULL)
        {
            variableStyleResult = _createInvalidResult("Variable style title cannot be NULL");
        }
        else
        {
            ValidationResult titleResult = _validateStyleList(style->variable_style->title, config);
            if (!titleResult.succeed)
            {
                variableStyleResult = _createInvalidResult("Variable style title contains invalid CSS");
                releaseValidationResult(&titleResult);
            }
            else
            {
                releaseValidationResult(&titleResult);
            }
        }

        if (style->variable_style->description != NULL)
        {
            ValidationResult descResult = _validateStyleList(style->variable_style->description, config);
            if (!descResult.succeed)
            {
                if (variableStyleResult.succeed)
                {
                    variableStyleResult = _createInvalidResult("Variable style description contains invalid CSS");
                }
                else
                {
                    // Combinar con el error existente
                    ValidationResult results[] = {variableStyleResult, _createInvalidResult("Variable style description contains invalid CSS")};
                    variableStyleResult = combineValidationResults(results, 2);
                }
            }
            releaseValidationResult(&descResult);
        }
    }

    // Combinar resultados de ambos estilos
    ValidationResult results[] = {methodStyleResult, variableStyleResult};
    return combineValidationResults(results, 2);
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
        "string", "int", "float", "boolean", "object", "array", "number", "void", "Date", "function"
    };

    int num_types = sizeof(valid_types) / sizeof(valid_types[0]);
    for (int i = 0; i < num_types; i++)
    {
        if (strcmp(type, valid_types[i]) == 0)
        {
            return true;
        }
    }

    // Verificar si es un número (no permitido como tipo)
    boolean is_numeric = true;
    for (int i = 0; type[i] != '\0'; i++)
    {
        if (!isdigit(type[i]) && type[i] != '.' && type[i] != '-')
        {
            is_numeric = false;
            break;
        }
    }
    if (is_numeric)
    {
        return false;
    }

    // Verificar si es un valor booleano (no permitido como tipo)
    if (strcmp(type, "true") == 0 || strcmp(type, "false") == 0)
    {
        return false;
    }

    // Solo permitir los tipos predefinidos - rechazar cualquier tipo personalizado
    return false;
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
    int brace_count = 0;

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
        case '{':
            brace_count++;
            break;
        case '}':
            brace_count--;
            if (brace_count < 0) return false;
            break;
        case '\\':
            // Verificar secuencias de escape válidas
            if (regex[i + 1] == '\0') return false; // Backslash al final
            i++; // Saltar el siguiente carácter
            break;
        }
    }

    // Verificar que todos los delimitadores estén balanceados
    if (bracket_count != 0 || paren_count != 0 || brace_count != 0)
    {
        return false;
    }

    return true;
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
    if (dash != NULL && dash != range && dash[1] != '\0')
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
        size_t content_len = strlen(range) - 2;
        if (content_len == 0) return false; // "[,]" no es válido
        
        char *content = malloc(content_len + 1);
        if (content == NULL) return false; // Error de memoria
        
        strncpy(content, range + 1, content_len);
        content[content_len] = '\0';
        
        // Buscar la coma
        char *comma = strchr(content, ',');
        if (comma != NULL && comma != content && comma[1] != '\0')
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