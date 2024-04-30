#include "Vector.h"
#include "error_management.h"
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
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
} ThreadProductArgs;

typedef struct
{
	Vector* v1;
	Vector* v2;
	Vector* res;
	size_t start;
	size_t end;
} ThreadSumArgs;

Vector* Vector_new(size_t size)
{
	Vector* res = (Vector*)malloc(sizeof(Vector));
	ASSERT(res, MEMORY_NOT_ASSIGNED_EXCEPTION);
	size_t size_in_bytes = size * sizeof(VEC_T);
	res->Buffer = (VEC_T*)malloc(size_in_bytes);
	ASSERT(res->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res->size = size;

	return res;
}

Vector* Vector_new_emplace(size_t size, ...)
{
	Vector* newVector = Vector_new(size);
	ASSERT(newVector, MEMORY_NOT_ASSIGNED_EXCEPTION);

	va_list args;
	va_start(args, size);

	for (size_t i = 0; i < size; i++)
	{
		VEC_T value = (VEC_T)va_arg(args, size_t);
		newVector->Buffer[i] = value;
	}

	va_end(args);

	return newVector;
}

Vector* Vector_new_container(size_t size)
{
	size_t capacity = (size + 2) * sizeof(VEC_T);
	Vector* res = (Vector*)malloc(sizeof(Vector));
	ASSERT(res, MEMORY_NOT_ASSIGNED_EXCEPTION);

	res->Buffer = (VEC_T*)malloc(capacity);
	ASSERT(res->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res->size = 0;
	res->capacity_in_bytes = capacity;

	return res;
}

Vector* Vector_new_zero(size_t size)
{
	Vector* res = Vector_new(size);
	ASSERT(res, MEMORY_NOT_ASSIGNED_EXCEPTION);
	ASSERT(res->Buffer, MEMORY_POINTING_TO_NOTHING);

	memset(res->Buffer, 0, size * sizeof(VEC_T));

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

Vector* Vector_new_move(Vector* oldVector)
{
	Vector* newVec = Vector_new_copy(oldVector);

	Vector_delete(oldVector);

	return newVec;
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

Vector Vector_create(size_t size)
{
	Vector res;

	size_t size_in_bytes = size * sizeof(VEC_T);
	res.Buffer = (VEC_T*)malloc(size_in_bytes);
	ASSERT(res.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res.size = size;

	return res;
}

Vector Vector_create_emplace(size_t size, ...)
{
	Vector v = Vector_create(size);

	va_list args;
	va_start(args, size);

	for (size_t i = 0; i < size; i++)
	{
		VEC_T value = (VEC_T)va_arg(args, size_t);
		v.Buffer[i] = value;
	}

	va_end(args);

	return v;
}

Vector Vector_create_container(size_t size)
{
	Vector res;
	size_t capacity = (size + 2) * sizeof(VEC_T);

	res.Buffer = (VEC_T*)malloc(capacity);
	ASSERT(res.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res.size = 0;
	res.capacity_in_bytes = capacity;

	return res;
}

Vector Vector_create_zero(size_t size)
{
	Vector res = Vector_create(size);

	memset(res.Buffer, 0, size * sizeof(VEC_T));

	return res;
}

Vector Vector_create_random(Vector_Datatype datatype, void* min, void* max, size_t size)
{
	size_t sizeVector = size * sizeof(VEC_T);

	Vector v;

	v.Buffer = (VEC_T*)malloc(sizeVector);
	ASSERT(v.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	if (datatype == INTEGER_VECTOR)
	{
		int a = *((int*)min);
		int b = *((int*)max);

		for (size_t i = 0; i < size; i++)
			v.Buffer[i] = genRandomInt(a, b);
	}
	else if (datatype == REAL_VECTOR)
	{
		VEC_T a = *((VEC_T*)min);
		VEC_T b = *((VEC_T*)max);

		for (size_t i = 0; i < size; i++)
			v.Buffer[i] = genRandomDouble(a, b);
	}
	v.size = size;

	return v;
}

Vector Vector_create_copy(Vector* newVector)
{
	ASSERT(newVector->Buffer, MEMORY_POINTING_TO_NOTHING);

	Vector v;
	Vector* vec = &v;

	size_t dim = newVector->size;
	vec->Buffer = (VEC_T*)malloc(sizeof(VEC_T) * dim);
	ASSERT(vec->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	memmove(vec->Buffer, newVector->Buffer, dim * sizeof(VEC_T));

	vec->size = newVector->size;

	return *vec;
}

Vector Vector_create_move(Vector* oldVector)
{
	ASSERT(oldVector->Buffer, MEMORY_POINTING_TO_NOTHING);

	Vector newVec;

	size_t dim = oldVector->size;
	newVec.Buffer = (VEC_T*)malloc(sizeof(VEC_T) * dim);
	ASSERT(newVec.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	memmove(newVec.Buffer, oldVector->Buffer, dim * sizeof(VEC_T));
	free(oldVector->Buffer);

	return newVec;
}

Vector Vector_create_array(VEC_T* newVector, size_t size)
{
	ASSERT(newVector, MEMORY_POINTING_TO_NOTHING);

	size_t sizeVector = size * sizeof(VEC_T);
	VEC_T* buffer = (VEC_T*)malloc(sizeVector);
	memcpy_s(buffer, sizeVector, newVector, sizeVector);

	Vector v;
	Vector* vector = &v;

	ASSERT(vector, MEMORY_NOT_ASSIGNED_EXCEPTION);

	vector->Buffer = buffer;
	vector->size = size;

	return *vector;
}

void Vector_add(Vector* self, VEC_T val)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);

	size_t sizeOfData = sizeof(VEC_T);
	
	size_t capacity_in_bytes = self->capacity_in_bytes;
	size_t current_size_in_bytes = self->size * sizeOfData;

	if (capacity_in_bytes < current_size_in_bytes + sizeOfData)
	{
		capacity_in_bytes += 2 * sizeOfData;
		self->capacity_in_bytes = capacity_in_bytes;
		VEC_T* tmp = NULL;
		while (tmp == NULL)
			tmp = (VEC_T*)realloc(self->Buffer, capacity_in_bytes);
		self->Buffer = tmp;
	}

	size_t newSize = self->size;
	ASSERT(self->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	self->Buffer[newSize++] = val;
	self->size = newSize;
}

void Vector_delete(Vector* vec)
{
	if (vec != NULL)
	{
		if (vec->Buffer != NULL)
		{
			free(vec->Buffer);
			vec->Buffer = NULL;
		}
		free(vec);
	}
	else
		free(vec);

	vec = NULL;
}

void Vector_delete_stacked(Vector* vec)
{
	if (vec->Buffer != NULL)
	{
		free(vec->Buffer);
		vec->Buffer = NULL;
	}
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

void Vector_move(Vector* dest, Vector* src)
{
	Vector_set_another_vector(dest, src);

	Vector_delete(src);
}

void Vector_move_stacked(Vector* dest, Vector* src)
{
	Vector_set_another_vector(dest, src);

	Vector_delete_stacked(src);
}

void Vector_set_another_vector(Vector* dest, Vector* src)
{
	ASSERT(dest, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(src, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(dest->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(src->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(dest->size == src->size, ARRAY_INVALID_DIMS_EXCEPTION);

	dest->capacity_in_bytes = src->capacity_in_bytes;

	memmove(dest->Buffer, src->Buffer, dest->size * sizeof(VEC_T));
}

VEC_T Vector_sum_element(Vector* arr, size_t index, VEC_T val)
{
	ASSERT(arr, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(arr->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(index < arr->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	arr->Buffer[index] += val;
	return arr->Buffer[index];
}

void* vectors_sum(ThreadSumArgs* args)
{
	Vector* v1 = args->v1;
	Vector* v2 = args->v2;
	Vector* res = args->res;
	size_t start = args->start;
	size_t end = args->end;

	// Suma entre los vectores
	for (size_t i = start; i < end; i++)
	{
		VEC_T v1Elem = Vector_get(v1, i);
		VEC_T v2Elem = Vector_get(v2, i);
		Vector_set(res, i, v1Elem + v2Elem);
	}

	pthread_exit(NULL);

	return NULL;
}

void vectors_sum_traditional(Vector* res, Vector* v1, Vector* v2)
{
	for (size_t i = 0; i < v1->size; i++)
	{
		VEC_T v1Elem = Vector_get(v1, i);
		VEC_T v2Elem = Vector_get(v2, i);
		Vector_set(res, i, v1Elem + v2Elem);
	}
}

Vector* Vector_sum(Vector* v1, Vector* v2)
{
	ASSERT(v1, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v2, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v1->size == v2->size, ARRAY_INVALID_DIMS_EXCEPTION);
	ASSERT(v1->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(v2->Buffer, MEMORY_POINTING_TO_NOTHING);

	size_t sizeOfArrays = v1->size;

	Vector* res = Vector_new(sizeOfArrays);

#if defined WITH_THREADS

	size_t elements_per_thread = sizeOfArrays / NUM_THREADS;

	if (elements_per_thread)
	{
		pthread_t threads[NUM_THREADS];
		ThreadSumArgs thread_args[NUM_THREADS];

		for (size_t i = 0; i < NUM_THREADS; i++)
		{
			size_t start = i * elements_per_thread;
			size_t end = (i == NUM_THREADS - 1) ? sizeOfArrays : (i + 1) * elements_per_thread;

			thread_args[i].start = start;
			thread_args[i].end = end;
			thread_args[i].v1 = v1;
			thread_args[i].v2 = v2;
			thread_args[i].res = res;

			pthread_create(&threads[i], NULL, vectors_sum, (void*)&thread_args[i]);
		}

		for (size_t i = 0; i < NUM_THREADS; i++)
			pthread_join(threads[i], NULL);
	}
	else
		vectors_sum_traditional(res, v1, v2);

#else

	vectors_sum_traditional(res, v1, v2);

#endif

	return res;
}

Vector Vector_sum_stacked(Vector* v1, Vector* v2)
{
	ASSERT(v1, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v2, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v1->size == v2->size, ARRAY_INVALID_DIMS_EXCEPTION);
	ASSERT(v1->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(v2->Buffer, MEMORY_POINTING_TO_NOTHING);

	size_t sizeOfArrays = v1->size;

	Vector v = Vector_create(sizeOfArrays);
	Vector* res = &v;

#if defined WITH_THREADS

	size_t elements_per_thread = sizeOfArrays / NUM_THREADS;

	if (elements_per_thread)
	{
		pthread_t threads[NUM_THREADS];
		ThreadSumArgs thread_args[NUM_THREADS];

		for (size_t i = 0; i < NUM_THREADS; i++)
		{
			size_t start = i * elements_per_thread;
			size_t end = (i == NUM_THREADS - 1) ? sizeOfArrays : (i + 1) * elements_per_thread;

			thread_args[i].start = start;
			thread_args[i].end = end;
			thread_args[i].v1 = v1;
			thread_args[i].v2 = v2;
			thread_args[i].res = res;

			pthread_create(&threads[i], NULL, vectors_sum, (void*)&thread_args[i]);
		}

		for (size_t i = 0; i < NUM_THREADS; i++)
			pthread_join(threads[i], NULL);
	}
	else
		vectors_sum_traditional(res, v1, v2);

#else

	vectors_sum_traditional(res, v1, v2);

#endif

	return *res;
}

void* matrix_vector_multiplication(ThreadProductArgs* args)
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

Vector* Vector_product_with_scalar(Vector* v, VEC_T scalar)
{
	ASSERT(v, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	Vector* res = Vector_new_copy(v);

	for (size_t i = 0; i < v->size; i++)
	{
		VEC_T product = Vector_get(v, i) * scalar;
		Vector_set(res, i, product);
	}

	return res;
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
		ThreadProductArgs thread_args[NUM_THREADS];
	
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

Vector Vector_mat_vec_product_stacked(struct mat* mat, Vector* v)
{
	ASSERT(v, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
	ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(v->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(mat->columns == v->size, MATRIX_INVALID_MULTIPLICATION);

	Vector v_stacked = Vector_create_zero(mat->rows);
	Vector* res = &v_stacked;

#if defined WITH_THREADS

	size_t elements_per_thread = v->size / NUM_THREADS;

	if (elements_per_thread)
	{
		pthread_t threads[NUM_THREADS];
		ThreadProductArgs thread_args[NUM_THREADS];

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

	return *res;
}

void Vector_sum_void(Vector* v1, Vector* v2)
{
	ASSERT(v1, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v2, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v1->size == v2->size, ARRAY_INVALID_DIMS_EXCEPTION);
	ASSERT(v1->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(v2->Buffer, MEMORY_POINTING_TO_NOTHING);

	size_t sizeOfArrays = v1->size;

#if defined WITH_THREADS

	size_t elements_per_thread = sizeOfArrays / NUM_THREADS;

	if (elements_per_thread)
	{
		pthread_t threads[NUM_THREADS];
		ThreadSumArgs thread_args[NUM_THREADS];

		for (size_t i = 0; i < NUM_THREADS; i++)
		{
			size_t start = i * elements_per_thread;
			size_t end = (i == NUM_THREADS - 1) ? sizeOfArrays : (i + 1) * elements_per_thread;

			thread_args[i].start = start;
			thread_args[i].end = end;
			thread_args[i].v1 = v1;
			thread_args[i].v2 = v2;
			thread_args[i].res = v1;

			pthread_create(&threads[i], NULL, vectors_sum, (void*)&thread_args);
		}

		for (size_t i = 0; i < NUM_THREADS; i++)
			pthread_join(threads[i], NULL);
	}
	else
		vectors_sum_traditional(v1, v1, v2);

#else

	vectors_sum_traditional(v1, v1, v2);

#endif
}

void Vector_product_with_scalar_void(Vector* v, VEC_T scalar)
{
	ASSERT(v, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(v->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	for (size_t i = 0; i < v->size; i++)
	{
		VEC_T product = Vector_get(v, i) * scalar;
		Vector_set(v, i, product);
	}
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
