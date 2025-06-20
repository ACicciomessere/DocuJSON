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
static void _generateMethod(Method *method, unsigned int indent);
static void _generateVariables(VariablesTitle *varsTitle, unsigned int indent);
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
    if (program->methods)
    {
        _output(2, "<div>\n");
        _output(3, "<h1>Métodos</h1>\n");
        MethodList *ml = program->methods->methods;
        while (ml)
        {
            _generateMethod(ml->method, 3);
            ml = ml->next;
        }
        _output(2, "</div>\n");
    }

    if (program->variables)
    {
        _generateVariables(program->variables, 2);
    }
}

static void _generateMethod(Method *method, unsigned int indent)
{
    MethodContent *c = method->content;
    _output(indent, "<div>\n");
    _output(indent + 1, "<h2>%s</h2>\n", method->name);
    _output(indent + 1, "<p><strong>Descripción:</strong> %s</p>\n", c->description);
    _output(indent + 1, "<p><strong>Tipo:</strong> %s</p>\n", c->type);

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
            _output(indent + 2, "<li>\n");
            _output(indent + 3, "<strong>%s</strong>\n", p->name);
            _output(indent + 3, "<ul>\n");
            _output(indent + 4, "<li>Descripción: %s</li>\n", d->description);
            _output(indent + 4, "<li>Tipo: %s</li>\n", d->type);
            _output(indent + 4, "<li>Regex: %s</li>\n", d->regex);
            _output(indent + 4, "<li>Rango: %s</li>\n", d->range);
            _output(indent + 3, "</ul>\n");
            _output(indent + 2, "</li>\n");
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
            _output(indent + 2, "<li>\n");
            _output(indent + 3, "<strong>%s</strong>\n", v->name);
            _output(indent + 3, "<ul>\n");
            _output(indent + 4, "<li>Descripción: %s</li>\n", d->description);
            _output(indent + 4, "<li>Tipo: %s</li>\n", d->type);
            _output(indent + 3, "</ul>\n");
            _output(indent + 2, "</li>\n");
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

static void _generateVariables(VariablesTitle *varsTitle, unsigned int indent)
{
    _output(indent, "<div>\n");
    _output(indent + 1, "<h1>Variables Globales</h1>\n");
    _output(indent + 1, "<div class=\"variable\">\n");
    VariableList *vl = varsTitle->variables;
    while (vl)
    {
        Variable *v = vl->variable;
        VariableData *d = v->data;
        _output(indent + 2, "<h3>%s</h3>\n", v->name);
        _output(indent + 2, "<p>Descripción: %s</p>\n", d->description);
        _output(indent + 2, "<p>Tipo: %s</p>\n", d->type);
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
    _output(1, "<body>\n");
    _output(2, "<h1>Documentación</h1>\n");
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
