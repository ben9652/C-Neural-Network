#ifndef MATRIX_H
#define MATRIX_H
#include <stdlib.h>
#include "Vector.h"

struct vec;

typedef double MAT_T;

struct mat {
	MAT_T* Buffer;
	size_t  rows;
	size_t columns;
};

typedef struct mat Matrix;

typedef enum {
    INTEGER_MATRIX,
    REAL_MATRIX
} Matrix_Datatype;

typedef struct {
    size_t i;
    size_t j;
} Matrix_Position;

inline Matrix_Position M_Pos(size_t i, size_t j);

/* Los "new" serán para la creación en el heap */

Matrix* Matrix_new(size_t rows, size_t columns);
Matrix* Matrix_new_copy(Matrix* newMatrix);
Matrix* Matrix_new_array(MAT_T* newMatrix, size_t rows, size_t columns);
Matrix* Matrix_new_null(size_t rows, size_t columns);
Matrix* Matrix_new_identity(size_t order);
Matrix* Matrix_new_random(Matrix_Datatype datatype, void* min, void* max, size_t rows, size_t columns);

/* Los "create" serán para la creación en el stack */

Matrix Matrix_create(size_t rows, size_t columns);
Matrix Matrix_create_copy(MAT_T* newMatrix, size_t rows, size_t columns);
Matrix Matrix_create_null(size_t rows, size_t columns);
Matrix Matrix_create_identity(size_t order);
Matrix Matrix_create_random(Matrix_Datatype datatype, size_t rows, size_t columns);

/* Constructores de copias */

Matrix* Matrix_new_duplicate(Matrix* matrix);
Matrix Matrix_create_duplicate(Matrix* matrix);

/* Creación de arreglos */

struct vec* Matrix_get_row(Matrix* mat, size_t row);

// Destructor
void Matrix_delete(Matrix* matrix);

// Setters y Getters
inline void Matrix_set(Matrix* matrix, Matrix_Position pos, MAT_T element);
inline MAT_T Matrix_get(Matrix* matrix, Matrix_Position pos);

void Matrix_set_row(Matrix* matrix, size_t row_index, MAT_T* row, size_t size);

/* Operaciones con matrices */

Matrix* Matrix_sum(Matrix* mat1, Matrix* mat2);
Matrix* Matrix_mul(Matrix* mat1, Matrix* mat2);
Matrix* Matrix_transpose(Matrix* mat);

// Muestra de la matriz
void Matrix_show(Matrix* mat);


#endif //MATRIX_H