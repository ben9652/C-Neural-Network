#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>
#include "Matrix.h"

struct mat;

typedef double VEC_T;

struct vec
{
	VEC_T* Buffer;
	size_t size;
	size_t capacity_in_bytes;
};

typedef struct vec Vector;

typedef enum {
	INTEGER_VECTOR,
	REAL_VECTOR
} Vector_Datatype;

/// <summary>
/// Vector creado como arreglo normal
/// </summary>
/// <param name="size">Tamaño fijo del arreglo</param>
/// <returns></returns>
Vector* Vector_new(size_t size);

Vector* Vector_new_emplace(size_t size, ...);

/// <summary>
/// Vector creado como contenedor para ir añadiendo elementos uno a uno, incluso si se supera el tamaño máximo
/// </summary>
/// <param name="size">Tamaño inicial para asignarle al contenedor</param>
/// <returns></returns>
Vector* Vector_new_container(size_t size);

Vector* Vector_new_zero(size_t size);
Vector* Vector_new_random(Vector_Datatype datatype, void* min, void* max, size_t size);
Vector* Vector_new_copy(Vector* newVector);
Vector* Vector_new_move(Vector* oldVector);
Vector* Vector_new_array(VEC_T* newVector, size_t size);

Vector Vector_create(size_t size);
Vector Vector_create_emplace(size_t size, ...);
Vector Vector_create_container(size_t size);
Vector Vector_create_zero(size_t size);
Vector Vector_create_random(Vector_Datatype datatype, void* min, void* max, size_t size);
Vector Vector_create_copy(Vector* newVector);
Vector Vector_create_move(Vector* oldVector);
Vector Vector_create_array(VEC_T* newVector, size_t size);

/// <summary>
/// Agregar nuevo elemento en un vector creado como contenedor
/// </summary>
/// <param name="self">Contenedor</param>
/// <param name="val">Número a agregar</param>
void Vector_add(Vector* self, VEC_T val);

void Vector_delete(Vector* vec);
void Vector_delete_stacked(Vector* vec);

inline VEC_T Vector_get(Vector* arr, size_t index);
inline void Vector_set(Vector* arr, size_t index, VEC_T val);
void Vector_move(Vector* dest, Vector* src);
void Vector_move_stacked(Vector* dest, Vector* src);
void Vector_set_another_vector(Vector* dest, Vector* src);
VEC_T Vector_sum_element(Vector* arr, size_t index, VEC_T val);

/* Operaciones que devuelven un vector nuevo */

Vector* Vector_sum(Vector* v1, Vector* v2);
Vector Vector_sum_stacked(Vector* v1, Vector* v2);
Vector* Vector_dot_product(Vector* v1, Vector* v2);
Vector* Vector_cross_product(Vector* v1, Vector* v2);
Vector* Vector_product_with_scalar(Vector* v, VEC_T scalar);
Vector* Vector_mat_vec_product(struct mat* mat, Vector* v);
Vector Vector_mat_vec_product_stacked(struct mat* mat, Vector* v);

/* Operaciones que modifican al primer vector */

void Vector_sum_void(Vector* v1, Vector* v2);
void Vector_dot_product_void(Vector* v1, Vector* v2);
void Vector_cross_product_void(Vector* v1, Vector* v2);
void Vector_product_with_scalar_void(Vector* v, VEC_T scalar);

void Vector_show(Vector* arr);


#endif