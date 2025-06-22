#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Función para eliminar espacios en blanco
char* trim(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// Función para validar un selector CSS básico
bool is_valid_selector(const char* selector) {
    if (!selector || strlen(selector) == 0) return false;
    
    // Permitir selectores básicos: elementos, clases, ids, atributos
    const char* valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_#.[]():> +~*";
    
    for (int i = 0; selector[i]; i++) {
        if (!strchr(valid_chars, selector[i])) {
            return false;
        }
    }
    return true;
}

// Función para validar una propiedad CSS
bool is_valid_property(const char* property) {
    if (!property || strlen(property) == 0) return false;
    
    // Lista de propiedades CSS comunes
    const char* valid_properties[] = {
        "color", "background-color", "font-size", "font-family", "font-weight",
        "margin", "margin-top", "margin-right", "margin-bottom", "margin-left",
        "padding", "padding-top", "padding-right", "padding-bottom", "padding-left",
        "border", "border-top", "border-right", "border-bottom", "border-left",
        "border-width", "border-style", "border-color", "border-radius",
        "width", "height", "max-width", "max-height", "min-width", "min-height",
        "display", "position", "top", "right", "bottom", "left", "z-index",
        "float", "clear", "overflow", "visibility", "opacity",
        "text-align", "text-decoration", "text-transform", "line-height",
        "vertical-align", "white-space", "letter-spacing", "word-spacing",
        "list-style", "list-style-type", "list-style-position", "list-style-image",
        "cursor", "outline", "box-shadow", "text-shadow", "transform",
        "transition", "animation", "flex", "flex-direction", "justify-content",
        "align-items", "grid", "grid-template-columns", "grid-template-rows"
    };
    
    int num_properties = sizeof(valid_properties) / sizeof(valid_properties[0]);
    
    for (int i = 0; i < num_properties; i++) {
        if (strcmp(property, valid_properties[i]) == 0) {
            return true;
        }
    }
    
    // Permitir propiedades personalizadas (que empiecen con --)
    if (strncmp(property, "--", 2) == 0) {
        return true;
    }
    
    return false;
}

// Función para validar un valor CSS básico
bool is_valid_value(const char* value) {
    if (!value || strlen(value) == 0) return false;
    
    // Permitir valores que contengan: letras, números, espacios, guiones, puntos,
    // porcentajes, píxeles, colores hex, paréntesis, comas, etc.
    const char* valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -_.,%#()/'\"!:;";
    
    for (int i = 0; value[i]; i++) {
        if (!strchr(valid_chars, value[i])) {
            return false;
        }
    }
    return true;
}

// Función para validar propiedades CSS individuales (formato: "prop1: val1; prop2: val2;")
bool validate_css_properties(const char* css_string) {
    if (!css_string) return false;
    
    char* css_copy = strdup(css_string);
    if (!css_copy) return false;
    
    // Verificar que termine con punto y coma
    char* trimmed_css = trim(css_copy);
    int len = strlen(trimmed_css);
    if (len == 0) {
        free(css_copy);
        return false;
    }
    
    // Si no termina con punto y coma, no es válido
    if (trimmed_css[len - 1] != ';') {
        free(css_copy);
        return false;
    }
    
    // Remover el último punto y coma para procesar
    trimmed_css[len - 1] = '\0';
    
    char* token = strtok(trimmed_css, ";");
    while (token != NULL) {
        // Limpiar espacios al inicio y final
        char* trimmed_token = trim(token);
        
        // Si el token está vacío después de limpiar, continuar
        if (strlen(trimmed_token) == 0) {
            token = strtok(NULL, ";");
            continue;
        }
        
        // Buscar los dos puntos que separan propiedad de valor
        char* colon = strchr(trimmed_token, ':');
        if (colon == NULL) {
            free(css_copy);
            return false; // No hay dos puntos
        }
        
        // Separar propiedad y valor
        *colon = '\0';
        char* property = trim(trimmed_token);
        char* value = trim(colon + 1);
        
        // Validar propiedad
        if (!is_valid_property(property)) {
            free(css_copy);
            return false;
        }
        
        // Validar valor
        if (!is_valid_value(value)) {
            free(css_copy);
            return false;
        }
        
        token = strtok(NULL, ";");
    }
    
    free(css_copy);
    return true;
}

// Función principal para validar CSS
bool validate_css(const char* css_string) {
    if (!css_string) return false;
    
    // Si contiene llaves, es CSS completo con selectores
    if (strchr(css_string, '{') != NULL) {
        return validate_css_complete(css_string);
    }
    
    // Si no contiene llaves, son propiedades CSS individuales
    return validate_css_properties(css_string);
}

// Función para validar CSS completo con selectores
bool validate_css_complete(const char* css_string) {
    if (!css_string) return false;
    
    char* css_copy = strdup(css_string);
    if (!css_copy) return false;
    
    char* ptr = css_copy;
    ParseState state = STATE_SELECTOR;
    char selector[1024] = {0};
    char property[256] = {0};
    char value[1024] = {0};
    int brace_count = 0;
    bool in_string = false;
    char string_char = 0;
    
    while (*ptr) {
        // Manejar comentarios CSS /* ... */
        if (!in_string && *ptr == '/' && *(ptr + 1) == '*') {
            ptr += 2;
            while (*ptr && !(*ptr == '*' && *(ptr + 1) == '/')) {
                ptr++;
            }
            if (*ptr) ptr += 2;
            continue;
        }
        
        // Manejar strings
        if ((*ptr == '"' || *ptr == '\'') && !in_string) {
            in_string = true;
            string_char = *ptr;
        } else if (*ptr == string_char && in_string) {
            in_string = false;
            string_char = 0;
        }
        
        if (!in_string) {
            switch (state) {
                case STATE_SELECTOR:
                    if (*ptr == '{') {
                        brace_count++;
                        // Validar selector
                        char* trimmed_selector = trim(selector);
                        if (!is_valid_selector(trimmed_selector)) {
                            free(css_copy);
                            return false;
                        }
                        memset(selector, 0, sizeof(selector));
                        state = STATE_PROPERTY;
                    } else if (*ptr != '\n' && *ptr != '\r') {
                        strncat(selector, ptr, 1);
                    }
                    break;
                    
                case STATE_PROPERTY:
                    if (*ptr == ':') {
                        // Validar propiedad
                        char* trimmed_property = trim(property);
                        if (!is_valid_property(trimmed_property)) {
                            free(css_copy);
                            return false;
                        }
                        memset(property, 0, sizeof(property));
                        state = STATE_VALUE;
                    } else if (*ptr == '}') {
                        brace_count--;
                        if (brace_count < 0) {
                            free(css_copy);
                            return false;
                        }
                        state = STATE_SELECTOR;
                        memset(property, 0, sizeof(property));
                    } else if (!isspace(*ptr)) {
                        strncat(property, ptr, 1);
                    }
                    break;
                    
                case STATE_VALUE:
                    if (*ptr == ';') {
                        // Validar valor
                        char* trimmed_value = trim(value);
                        if (!is_valid_value(trimmed_value)) {
                            free(css_copy);
                            return false;
                        }
                        memset(value, 0, sizeof(value));
                        state = STATE_PROPERTY;
                    } else if (*ptr == '}') {
                        // Validar último valor antes del cierre
                        if (strlen(value) > 0) {
                            char* trimmed_value = trim(value);
                            if (!is_valid_value(trimmed_value)) {
                                free(css_copy);
                                return false;
                            }
                        }
                        brace_count--;
                        if (brace_count < 0) {
                            free(css_copy);
                            return false;
                        }
                        state = STATE_SELECTOR;
                        memset(value, 0, sizeof(value));
                    } else {
                        strncat(value, ptr, 1);
                    }
                    break;
            }
        }
        
        ptr++;
    }
    
    free(css_copy);
    
    // Verificar que todas las llaves estén balanceadas
    return brace_count == 0;
}