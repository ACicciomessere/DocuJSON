#ifndef UTILS_H
#define UTILS_H

// C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Estados del parser
typedef enum {
    STATE_SELECTOR,
    STATE_PROPERTY,
    STATE_VALUE,
    STATE_COMMENT
} ParseState;

char* trim(char* str);
bool is_valid_selector(const char* selector);
bool is_valid_property(const char* property);
bool is_valid_value(const char* value);
bool validate_css(const char* css_string);
bool validate_css_properties(const char* css_string);
bool validate_css_complete(const char* css_string);

#endif