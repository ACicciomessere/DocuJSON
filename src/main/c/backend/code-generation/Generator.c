#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger *_logger = NULL;

void initializeGeneratorModule()
{
    _logger = createLogger("Generator");
}

void shutdownGeneratorModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }
}

/** PRIVATE FUNCTIONS */

// static const char _expressionTypeToCharacter(const ExpressionType type);
// static void _generateConstant(const unsigned int indentationLevel, Constant *constant);
static void _generateEpilogue();
// static void _generateExpression(const unsigned int indentationLevel, Expression *expression);
// static void _generateFactor(const unsigned int indentationLevel, Factor *factor);
static void _generateProgram(Program *program);
static void _generateMethod(Method *method, unsigned int indent, char *mtitle_style, char *mdesc_style);
static void _generateVariables(VariablesTitle *varsTitle, unsigned int indent, char *vtitle_style, char *vdesc_style);
static void _generatePrologue(void);
static char *_indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char *const format, ...);

/**
 * Converts and expression type to the proper character of the operation
 * involved, or returns '\0' if that's not possible.
 */
// static const char _expressionTypeToCharacter(const ExpressionType type)
// {
//     switch (type)
//     {
//     case ADDITION:
//         return '+';
//     case DIVISION:
//         return '/';
//     case MULTIPLICATION:
//         return '*';
//     case SUBTRACTION:
//         return '-';
//     default:
//         logError(_logger, "The specified expression type cannot be converted into character: %d", type);
//         return '\0';
//     }
// }

/**
 * Generates the output of a constant.
 */
// static void _generateConstant(const unsigned int indentationLevel, Constant *constant)
// {
//     _output(indentationLevel, "%s", "[ $C$, circle, draw, black!20\n");
//     _output(1 + indentationLevel, "%s%d%s", "[ $", constant->value, "$, circle, draw ]\n");
//     _output(indentationLevel, "%s", "]\n");
// }

static void _generateEpilogue()
{
    _output(1, "</body>\n");
    _output(0, "</html>\n");
}

/**
 * Generates the output of an expression.
 */
// static void _generateExpression(const unsigned int indentationLevel, Expression *expression)
// {
//     _output(indentationLevel, "%s", "[ $E$, circle, draw, black!20\n");
//     switch (expression->type)
//     {
//     case ADDITION:
//     case DIVISION:
//     case MULTIPLICATION:
//     case SUBTRACTION:
//         _generateExpression(1 + indentationLevel, expression->leftExpression);
//         _output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
//         _generateExpression(1 + indentationLevel, expression->rightExpression);
//         break;
//     case FACTOR:
//         _generateFactor(1 + indentationLevel, expression->factor);
//         break;
//     default:
//         logError(_logger, "The specified expression type is unknown: %d", expression->type);
//         break;
//     }
//     _output(indentationLevel, "%s", "]\n");
// }

/**
 * Generates the output of a factor.
 */
// static void _generateFactor(const unsigned int indentationLevel, Factor *factor)
// {
//     _output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
//     switch (factor->type)
//     {
//     case CONSTANT:
//         _generateConstant(1 + indentationLevel, factor->constant);
//         break;
//     case EXPRESSION:
//         _output(1 + indentationLevel, "%s", "[ $($, circle, draw, purple ]\n");
//         _generateExpression(1 + indentationLevel, factor->expression);
//         _output(1 + indentationLevel, "%s", "[ $)$, circle, draw, purple ]\n");
//         break;
//     default:
//         logError(_logger, "The specified factor type is unknown: %d", factor->type);
//         break;
//     }
//     _output(indentationLevel, "%s", "]\n");
// }

/**
 * Generates the output of the program.
 */
static void _generateProgram(Program *program)
{
    char *style_method_title;
    char *style_method_desc;
    char *style_variable_title;
    char *style_variable_desc;

    if (program->style)
    {
        if (program->style->method_style)
        {
            style_method_title = program->style->method_style->title;
            style_method_desc = program->style->method_style->description;
        }
        if (program->style->variable_style)
        {
            style_variable_title = program->style->variable_style->title;
            style_variable_desc = program->style->variable_style->description;
        }
    }

    if (program->methods)
    {
        _output(2, "<div>\n");
        _output(3, "<h1>Métodos</h1>\n");
        MethodList *ml = program->methods->methods;

        while (ml)
        {
            _generateMethod(ml->method, 3, style_method_title, style_method_desc);
            ml = ml->next;
        }
        _output(2, "</div>\n");
    }

    if (program->variables)
    {
        _generateVariables(program->variables, 2, style_variable_title, style_variable_desc);
    }
}

static void _generateParam(Param *param, ParamData *data, unsigned int indent)
{
    _output(indent + 2, "<li>\n");
    _output(indent + 3, "<strong>%s</strong>\n", param->name);
    _output(indent + 3, "<ul>\n");
    _output(indent + 4, "<li>Descripción: %s</li>\n", data->description);
    _output(indent + 4, "<li>Tipo: <code>%s</code></li>\n", data->type);
    if (data->regex)
        _output(indent + 4, "<li>Regex: <code>%s</code></li>\n", data->regex);
    if (data->range)
        _output(indent + 4, "<li>Rango: <code>%s</code></li>\n", data->range);
    _output(indent + 3, "</ul>\n");
    _output(indent + 2, "</li>\n");
}

static void _generateVariable(Variable *var, VariableData *data, unsigned int indent)
{
    _output(indent + 2, "<li>\n");
    _output(indent + 3, "<strong>%s</strong>\n", var->name);
    _output(indent + 3, "<ul>\n");
    _output(indent + 4, "<li>Descripción: %s</li>\n", data->description);
    _output(indent + 4, "<li>Tipo: <code>%s</code></li>\n", data->type);
    _output(indent + 3, "</ul>\n");
    _output(indent + 2, "</li>\n");
}

static void _generateMethod(Method *method, unsigned int indent, char *mtitle_style, char *mdesc_style)
{
    MethodContent *c = method->content;
    _output(indent, "<div class=\"card\" id=\"%s\">\n", method->name);
    _output(indent + 1, "<h2 style=\"%s;\">%s</h2>\n", mtitle_style, method->name);
    _output(indent + 1, "<p style=\"%s;\">%s</p>\n", mdesc_style, c->description);
    _output(indent + 1, "<code>%s</code>\n", c->type);

    // Parámetros
    if (c->params && c->params->params)
    {
        _output(indent + 1, "<h3>Parámetros</h3>\n");
        _output(indent + 1, "<ul>\n");
        ParamsList *pl = c->params->params;
        while (pl)
        {
            Param *p = pl->param;
            ParamData *d = p->data;
            _generateParam(p, d, indent);
            pl = pl->next;
        }
        _output(indent + 1, "</ul>\n");
    }

    // Variables internas
    if (c->variables && c->variables->variables)
    {
        _output(indent + 1, "<h3>Variables Internas</h3>\n");
        _output(indent + 1, "<ul>\n");
        VariableList *vl = c->variables->variables;
        while (vl)
        {
            Variable *v = vl->variable;
            VariableData *d = v->data;
            _generateVariable(v, d, indent);

            vl = vl->next;
        }
        _output(indent + 1, "</ul>\n");
    }

    // Métodos relacionados
    if (c->related && c->related->related)
    {
        _output(indent + 1, "<h3>Métodos Relacionados</h3>\n");
        _output(indent + 1, "<ul>\n");
        RelatedList *rl = c->related->related;
        while (rl)
        {
            _output(indent + 2, "<li><a href=\"#%s\">%s</a></li>\n", rl->name, rl->name);
            rl = rl->next;
        }
        _output(indent + 1, "</ul>\n");
    }

    _output(indent, "</div>\n");
}

static void _generateVariables(VariablesTitle *varsTitle, unsigned int indent, char *vtitle_style, char *vdesc_style)
{
    _output(indent, "<div>\n");
    _output(indent + 1, "<h1>Variables Globales</h1>\n");
    _output(indent + 1, "<div class=\"card\">\n");
    VariableList *vl = varsTitle->variables;
    while (vl)
    {
        Variable *v = vl->variable;
        VariableData *d = v->data;

        _output(indent + 2, "<h3 style=\"%s;\">%s</h3>\n", vtitle_style, v->name);
        _output(indent + 2, "<p style=\"%s;\"> %s</p>\n", vdesc_style, d->description);
        _output(indent + 2, "<code>%s</code>\n", d->type);
        vl = vl->next;
    }
    _output(indent + 1, "</div>\n");
    _output(indent, "</div>\n");
}

static void _generatePrologue(void)
{
    _output(0, "<!DOCTYPE html>\n");
    _output(0, "<html lang=\"es\">\n");
    _output(1, "<head>\n");
    _output(2, "<meta charset=\"UTF-8\">\n");
    _output(2, "<title>Documentación</title>\n");
    _output(1, "</head>\n");
    _output(1, "<style>\n"
               "  body {\n"
               "    font-family: monospace;"
               "    background: #f5f7fa;\n"
               "    color: #333;\n"
               "    margin: 0;\n"
               "    padding: 2rem;\n"
               "  }\n"
               "  .card {\n"
               "    background: #fff;\n"
               "    padding: 1.5rem 2rem;\n"
               "    border-radius: 10px;\n"
               "    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);\n"
               "    margin-bottom: 2rem;\n"
               "  }\n"
               "  code {\n"
               "    background: #e9ecef;\n"
               "    padding: 0.02rem 0.2rem;\n"
               "    border-radius: 5px;\n"
               "    font-weight: bold;\n"
               "  }\n"
               "  .section-title {\n"
               "    font-size: 1.8rem;\n"
               "    color: #343a40;\n"
               "    margin-bottom: 1rem;\n"
               "  }\n"
               "</style>\n");

    _output(1, "<body>\n");
    _output(2, "<h1 style=\"text-align: center;\">Documentación</h1>\n");
}

/**
 * Generates an indentation string for the specified level.
 */
static char *_indentation(const unsigned int level)
{
    return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
static void _output(const unsigned int indentationLevel, const char *const format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *indentation = _indentation(indentationLevel);
    char *effectiveFormat = concatenate(2, indentation, format);
    vfprintf(stdout, effectiveFormat, arguments);
    fflush(stdout);
    free(effectiveFormat);
    free(indentation);
    va_end(arguments);
}

/** PUBLIC FUNCTIONS */

void generate(CompilerState *compilerState)
{
    logDebugging(_logger, "Generating final output...");
    _generatePrologue();
    _generateProgram(compilerState->abstractSyntaxtTree);
    _generateEpilogue();
    logDebugging(_logger, "Generation is done.");
}
