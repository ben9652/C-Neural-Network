#include "Vector.h"
#include "error_management.h"
#include <pthread.h>
#include <string.h>
#include "Random.h"

#define WITH_THREADS
#define NUM_THREADS 8

typedef struct
{
	Matrix* mat;
	Vector* v;
	Vector* res;
	size_t start;
	size_t end;
} ThreadArgs;

Vector* Vector_new_zero(size_t size)
{
	Vector* res = (Vector*)malloc(sizeof(Vector));
	ASSERT(res, MEMORY_NOT_ASSIGNED_EXCEPTION);

	size_t size_in_bytes = size * sizeof(VEC_T);
	res->Buffer = (VEC_T*)malloc(size_in_bytes);
	ASSERT(res->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res->size = size;

	memset(res->Buffer, 0, size_in_bytes);

	return res;
}

Vector* Vector_new_random(Vector_Datatype datatype, void* min, void* max, size_t size)
{
	size_t sizeVector = size * sizeof(VEC_T);

	Vector* vector = (Vector*)malloc(sizeof(Vector));
	ASSERT(vector, MATRIX_NOT_CREATED_EXCEPTION);

	vector->Buffer = (VEC_T*)malloc(sizeVector);
	ASSERT(vector->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	if (datatype == INTEGER_VECTOR)
	{
		int a = *((int*)min);
		int b = *((int*)max);

		for (size_t i = 0; i < size; i++)
			vector->Buffer[i] = genRandomInt(a, b);
	}
	else if (datatype == REAL_VECTOR)
	{
		VEC_T a = *((VEC_T*)min);
		VEC_T b = *((VEC_T*)max);

		for (size_t i = 0; i < size; i++)
			vector->Buffer[i] = genRandomDouble(a, b);
	}
	vector->size = size;

	return vector;
}

Vector* Vector_new_copy(Vector* newVector)
{
	ASSERT(newVector, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(newVector->Buffer, MEMORY_POINTING_TO_NOTHING);

	Vector* vec = (Vector*)malloc(sizeof(Vector));
	ASSERT(vec, MEMORY_NOT_ASSIGNED_EXCEPTION);

	size_t dim = newVector->size;
	vec->Buffer = (MAT_T*)malloc(sizeof(MAT_T) * dim);
	ASSERT(vec->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	memmove(vec->Buffer, newVector->Buffer, dim * sizeof(MAT_T));

	vec->size = newVector->size;

	return vec;
}

Vector* Vector_new_array(VEC_T* newVector, size_t size)
{
	ASSERT(newVector, MEMORY_POINTING_TO_NOTHING);

	size_t sizeVector = size * sizeof(VEC_T);
	VEC_T* buffer = (VEC_T*)malloc(sizeVector);
	memcpy_s(buffer, sizeVector, newVector, sizeVector);

	Vector* vector = (Vector*)malloc(sizeof(Vector));

	ASSERT(vector, MEMORY_NOT_ASSIGNED_EXCEPTION);

	vector->Buffer = buffer;
	vector->size = size;

	return vector;
}

void Vector_delete(Vector* vec)
{
	ASSERT(vec, ARRAY_NULL_PASSED_EXCEPTION);

	if(vec->Buffer != NULL)
		free(vec->Buffer);

	free(vec);
}

inline VEC_T Vector_get(Vector* arr, size_t index)
{
	ASSERT(arr, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(arr->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(index < arr->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	return arr->Buffer[index];
}

inline void Vector_set(Vector* arr, size_t index, VEC_T val)
{
	ASSERT(arr, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(arr->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(index < arr->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	arr->Buffer[index] = val;
}

void Vector_sum_element(Vector* arr, size_t index, VEC_T val)
{
	ASSERT(arr, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(arr->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(index < arr->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	arr->Buffer[index] += val;
}

void* matrix_vector_multiplication(ThreadArgs* args)
{
	Matrix* mat = args->mat;
	Vector* v = args->v;
	Vector* res = args->res;
	size_t start = args->start;
	size_t end = args->end;

	// Multiplicación de la matriz por el vector
	for (size_t i = start; i < end; i++)
	{
		for (size_t j = 0; j < res->size; j++)
		{
			MAT_T matElem = Matrix_get(mat, M_Pos(i, j));
			VEC_T vecElem = Vector_get(v, j);
			Vector_sum_element(res, i, matElem * vecElem);
		}
	}

	pthread_exit(NULL);

	return NULL;
}

void matrix_vector_multiplication_traditional(Vector* res, struct mat* mat, Vector* v)
{
	for (size_t i = 0; i < mat->rows; i++)
	{
		for (size_t j = 0; j < v->size; j++)
		{
			MAT_T matElem = Matrix_get(mat, M_Pos(i, j));
			VEC_T vecElem = Vector_get(v, j);
			Vector_sum_element(res, i, matElem * vecElem);
		}
	}
}

Vector* Vector_mat_vec_product(struct mat* mat, Vector* v)
{
	ASSERT(v, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
	ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(v->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(mat->columns == v->size, MATRIX_INVALID_MULTIPLICATION);

	Vector* res = Vector_new_zero(mat->rows);

#if defined WITH_THREADS

	size_t elements_per_thread = v->size / NUM_THREADS;

	if (elements_per_thread)
	{
		pthread_t threads[NUM_THREADS];
		ThreadArgs thread_args[NUM_THREADS];
	
		for (size_t i = 0; i < NUM_THREADS; i++)
		{
			size_t start = i * elements_per_thread;
			size_t end = (i == NUM_THREADS - 1) ? v->size : (i + 1) * elements_per_thread;

			thread_args[i].start = start;
			thread_args[i].end = end;
			thread_args[i].mat = mat;
			thread_args[i].v = v;
			thread_args[i].res = res;

			pthread_create(&threads[i], NULL, matrix_vector_multiplication, (void*)&thread_args);
		}

		for (size_t i = 0; i < NUM_THREADS; i++)
			pthread_join(threads[i], NULL);
	}
	else
		matrix_vector_multiplication_traditional(res, mat, v);

#else

	matrix_vector_multiplication_traditional(res, mat, v);

#endif

	return res;
}

void Vector_show(Vector* arr)
{
	for (size_t i = 0; i < arr->size; i++)
	{
		if (i == arr->size - 1)
			printf("%lf\n", arr->Buffer[i]);
		else
			printf("%lf, ", arr->Buffer[i]);
	}
}
