#ifndef STRING_H
#define STRING_H
#include <stdlib.h>

typedef struct vec_char
{
	char* Buffer;
	size_t size;
	size_t capacity_in_bytes;
} String;

/// <summary>
/// Cadena de caracteres creada como arreglo normal
/// </summary>
/// <param name="size">Tamaño fijo del arreglo</param>
/// <returns></returns>
String* const String_new(size_t size);
String String_create(size_t size);

String* const String_new_emplace(const char* string);
String String_create_emplace(const char* string);

/// <summary>
/// Cadena de caracteres creada como contenedor para ir añadiendo caracteres uno a uno, incluso si se supera el tamaño máximo
/// </summary>
/// <param name="size">Tamaño inicial para asignarle al contenedor</param>
/// <returns></returns>
String* const String_new_container(size_t size);
String String_create_container(size_t size);

String* const String_new_copy(const String* newString);
String String_create_copy(const String* newString);

/// <summary>
/// Agregar nuevo elemento en un String creado como contenedor
/// </summary>
/// <param name="self">Contenedor</param>
/// <param name="val">Número a agregar</param>
void String_add(String* const self, const char* string);

void String_add_char(String* const self, char c);
void String_backspace(String* const self);

void String_emplace(String* const self, int strings, ...);

void String_delete(String* self);
void String_delete_stacked(String* self);

inline char String_get(const String* self, size_t index);
inline void String_set(String* const self, size_t index, char val);
void String_clear(String* const self);
void String_set_another_String(String* const dest, const String* src);

String* const String_int_to_string(size_t num);
String String_int_to_string_stacked(size_t num);
void String_set_int_to_string(String* const self, size_t num);

String* const String_double_to_string(double num);
String String_double_to_string_stacked(double num);
void String_set_double_to_string(String* const self, double num);

void String_print(const String * str);


#endif