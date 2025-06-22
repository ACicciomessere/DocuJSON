#include "utils.h"

// Función para validar una propiedad CSS
boolean is_valid_property(const char* property) {
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
boolean is_valid_value(const char* value) {
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