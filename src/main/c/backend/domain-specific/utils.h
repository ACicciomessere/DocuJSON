#ifndef UTILS_H
#define UTILS_H

// C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../../shared/Type.h"

boolean is_valid_property(const char* property);
boolean is_valid_value(const char* value);

#endif