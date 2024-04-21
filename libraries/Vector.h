#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>
#include "Matrix.h"

typedef double MAT_T;

struct mat;

typedef double VEC_T;

typedef enum {
	INTEGER_VECTOR,
	REAL_VECTOR
} Vector_Datatype;

struct vec
{
	VEC_T* Buffer;
	size_t size;
};

typedef struct vec Vector;

Vector* Vector_new_zero(size_t size);
Vector* Vector_new_random(Vector_Datatype datatype, void* min, void* max, size_t size);
Vector* Vector_new_copy(Vector* newVector);
Vector* Vector_new_array(VEC_T* newVector, size_t size);

void Vector_delete(Vector* vec);

inline VEC_T Vector_get(Vector* arr, size_t index);
inline void Vector_set(Vector* arr, size_t index, VEC_T val);
void Vector_sum_element(Vector* arr, size_t index, VEC_T val);
Vector* Vector_dot_product(Vector* v1, Vector* v2);
Vector* Vector_cross_product(Vector* v1, Vector* v2);
Vector* Vector_mat_vec_product(struct mat* mat, Vector* v);

void Vector_show(Vector* arr);


#endif