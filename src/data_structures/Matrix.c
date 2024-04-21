#include "Matrix.h"
#include "error_management.h"
#include <string.h>
#include <stdlib.h>
#include "pthread.h"
#include "Random.h"

//#define PARALLEL_MULTIPLICATION
//#define TRANSPOSED_MATRIX_HELP
#define ADVANTAGING_WITH_CACHE_LINES

Matrix_Position M_Pos(size_t i, size_t j)
{
    Matrix_Position mat;
    mat.i = i;
    mat.j = j;

    return mat;
}

Matrix* Matrix_new(size_t rows, size_t columns)
{
    size_t sizeMatrix = rows * columns;
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix * sizeof(MAT_T));

    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));

    ASSERT(matrix, MATRIX_NOT_CREATED_EXCEPTION);

    matrix->Buffer = buffer;
    matrix->rows = rows;
    matrix->columns = columns;

    return matrix;
}

Matrix* Matrix_new_copy(Matrix* newMatrix)
{
    ASSERT(newMatrix, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(newMatrix->Buffer, MEMORY_POINTING_TO_NOTHING);

    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    ASSERT(mat, MEMORY_NOT_ASSIGNED_EXCEPTION);

    size_t dim = newMatrix->rows * newMatrix->columns;
    mat->Buffer = (MAT_T*)malloc(sizeof(MAT_T) * dim);
    ASSERT(mat->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
    memmove(mat->Buffer, newMatrix->Buffer, dim * sizeof(MAT_T));

    mat->rows = newMatrix->rows;
    mat->columns = newMatrix->columns;

    return mat;
}

Matrix* Matrix_new_array(MAT_T* newMatrix, size_t rows, size_t columns)
{
    ASSERT(newMatrix, MEMORY_POINTING_TO_NOTHING);

    size_t sizeMatrix = rows * columns * sizeof(MAT_T);
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix);
    memmove(buffer, newMatrix, sizeMatrix);
    
    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));

    ASSERT(matrix, MEMORY_NOT_ASSIGNED_EXCEPTION);

    matrix->Buffer = buffer;
    matrix->rows = rows;
    matrix->columns = columns;

    return matrix;
}

Matrix* Matrix_new_null(size_t rows, size_t columns)
{
    size_t sizeMatrix = rows * columns * sizeof(MAT_T);
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix);
    ASSERT(buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

    memset(buffer, 0, sizeMatrix);

    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));

    ASSERT(matrix, MATRIX_NOT_CREATED_EXCEPTION);

    matrix->Buffer = buffer;
    matrix->rows = rows;
    matrix->columns = columns;

    return matrix;
}

Matrix* Matrix_new_identity(size_t order)
{
    Matrix* new = Matrix_new_null(order, order);
    
    for (size_t i = 0; i < order; i++)
        Matrix_set(new, M_Pos(i, i), 1);

    return new;
}

Matrix* Matrix_new_random(Matrix_Datatype datatype, void* min, void* max, size_t rows, size_t columns)
{
    size_t sizeMatrix = rows * columns * sizeof(MAT_T);

    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));
    ASSERT(matrix, MATRIX_NOT_CREATED_EXCEPTION);

    matrix->Buffer = (MAT_T*)malloc(sizeMatrix);
    ASSERT(matrix->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

    size_t elementsQnty = rows * columns;

    if (datatype == INTEGER_MATRIX)
    {
        int a = *((int*)min);
        int b = *((int*)max);

        for (size_t i = 0; i < elementsQnty; i++)
            matrix->Buffer[i] = genRandomInt(a, b);
        matrix->rows = rows;
        matrix->columns = columns;
    }
    else if (datatype == REAL_MATRIX)
    {
        MAT_T a = *((MAT_T*)min);
        MAT_T b = *((MAT_T*)max);

        for (size_t i = 0; i < elementsQnty; i++)
            matrix->Buffer[i] = genRandomDouble(a, b);
        matrix->rows = rows;
        matrix->columns = columns;
    }

    return matrix;
}

Matrix Matrix_create(size_t rows, size_t columns)
{
    size_t sizeMatrix = rows * columns;
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix * sizeof(MAT_T));

    Matrix matrix;

    matrix.Buffer = buffer;
    matrix.rows = rows;
    matrix.columns = columns;

    return matrix;
}

Matrix Matrix_create_copy(MAT_T* newMatrix, size_t rows, size_t columns)
{
    ASSERT(newMatrix, MEMORY_POINTING_TO_NOTHING);

    size_t sizeMatrix = rows * columns * sizeof(MAT_T);
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix);
    memmove(buffer, newMatrix, sizeMatrix);

    Matrix matrix;

    matrix.Buffer = buffer;
    matrix.rows = rows;
    matrix.columns = columns;

    return matrix;
}

Matrix Matrix_create_null(size_t rows, size_t columns)
{
    size_t sizeMatrix = rows * columns * sizeof(MAT_T);
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix);
    ASSERT(buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

    memset(buffer, 0, sizeMatrix);

    Matrix matrix;

    matrix.Buffer = buffer;
    matrix.rows = rows;
    matrix.columns = columns;

    return matrix;
}

Matrix Matrix_create_identity(size_t order)
{
    Matrix new = Matrix_create_null(order, order);

    for (size_t i = 0; i < order; i++)
        Matrix_set(&new, M_Pos(i, i), 1);

    return new;
}

Matrix* Matrix_new_duplicate(Matrix* matrix)
{
    ASSERT(matrix, MEMORY_POINTING_TO_NOTHING);
    ASSERT(matrix->Buffer, MEMORY_POINTING_TO_NOTHING);

    size_t sizeMatrix = matrix->rows * matrix->columns * sizeof(MAT_T);
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix);
    ASSERT(buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

    Matrix* new = (Matrix*)malloc(sizeof(Matrix));
    ASSERT(new, MATRIX_NOT_CREATED_EXCEPTION);

    memmove(buffer, matrix->Buffer, sizeMatrix);
    new->Buffer = buffer;
    new->rows = matrix->rows;
    new->columns = matrix->columns;

    return new;
}

Matrix Matrix_create_duplicate(Matrix* matrix)
{
    ASSERT(matrix->Buffer, MEMORY_POINTING_TO_NOTHING);

    size_t sizeMatrix = matrix->rows * matrix->columns * sizeof(MAT_T);
    MAT_T* buffer = (MAT_T*)malloc(sizeMatrix);
    ASSERT(buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

    Matrix new;

    memmove(buffer, matrix->Buffer, sizeMatrix);
    new.Buffer = buffer;
    new.rows = matrix->rows;
    new.columns = matrix->columns;

    return new;
}

Vector* Matrix_get_row(Matrix* mat, size_t row)
{
    ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(row < mat->rows, MATRIX_OUT_OF_BOUNDS_EXCEPTION);

    Vector* new_array = (Vector*)malloc(sizeof(Vector));
    ASSERT(new_array, MEMORY_NOT_ASSIGNED_EXCEPTION);
    new_array->Buffer = mat->Buffer + row * mat->columns;
    new_array->size = mat->rows;

    return new_array;
}

void Matrix_delete(Matrix* matrix)
{
    ASSERT(matrix, MATRIX_NULL_PASSED_EXCEPTION);

    if(matrix->Buffer != NULL)
        free(matrix->Buffer);

    free(matrix);
}

void Matrix_set(Matrix* matrix, Matrix_Position pos, MAT_T element)
{
    ASSERT(matrix, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(matrix->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(pos.i < matrix->rows && pos.j < matrix->columns, MATRIX_OUT_OF_BOUNDS_EXCEPTION);

    size_t index = pos.j + pos.i * matrix->columns;
    matrix->Buffer[index] = element;
    return;
}

MAT_T Matrix_get(Matrix* matrix, Matrix_Position pos)
{
    ASSERT(matrix, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(matrix->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(pos.i < matrix->rows && pos.j < matrix->columns, MATRIX_OUT_OF_BOUNDS_EXCEPTION);

    size_t index = pos.j + pos.i * matrix->columns;
    return matrix->Buffer[index];
}

typedef struct {
    Matrix* result;
    Matrix* mat1;
    Matrix* mat2;
    size_t pivot;
    unsigned char firstPartition;
} Partial_Multiplication_Args;

// Función para la multiplicación parcial de la matriz para la implementación con dos hilos
void* partial_mul_Matrix(Partial_Multiplication_Args* args)
{
    Matrix* result = args->result;
    Matrix* mat1 = args->mat1;
    Matrix* mat2 = args->mat2;
    size_t pivot = args->pivot;
    unsigned char firstPartition = args->firstPartition;

    size_t initial = firstPartition ? 0 : pivot;
    size_t limit = firstPartition ? pivot : mat1->rows;

    Matrix* tmp = Matrix_transpose(mat2);
    for (size_t i = initial; i < limit; ++i)
    {
        //if(i % 10 == 0 && firstPartition == 1)
        //    printf("Here starts a new multiple of 10 i index: %zu\n", i);
        for (size_t j = 0; j < mat2->columns; ++j)
        {
            //if (j % 10 == 0 && firstPartition == 1)
            //    printf("Here starts a new multiple of j index: %zu\n", j);
            MAT_T sum = 0;
            for (size_t k = 0; k < mat2->rows; ++k)
            {
                //if (k % 10 == 0 && firstPartition == 1)
                //    printf("k: %zu\n", k);
                MAT_T elemMat1 = Matrix_get(mat1, M_Pos(i, k));
                MAT_T elemMat2 = Matrix_get(tmp, M_Pos(j, k));
                sum += elemMat1 * elemMat2;
            }

            Matrix_set(result, M_Pos(i, j), sum);
        }
    }

    return NULL;
}

Matrix* Matrix_sum(Matrix* mat1, Matrix* mat2)
{
    return NULL;
}

void Matrix_set_row(Matrix* matrix, size_t row_index, MAT_T* row, size_t size)
{
    ASSERT(matrix, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(row, MEMORY_POINTING_TO_NOTHING);
    ASSERT(row_index < matrix->rows, MATRIX_OUT_OF_BOUNDS_EXCEPTION);
    ASSERT(size <= matrix->columns, MATRIX_OUT_OF_BOUNDS_EXCEPTION);

    memmove(matrix->Buffer + row_index, row, size * sizeof(MAT_T));
}

Matrix* Matrix_mul(Matrix* mat1, Matrix* mat2)
{
    ASSERT(mat1, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat2, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat1->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(mat2->Buffer, MEMORY_POINTING_TO_NOTHING);

    ASSERT(mat1->columns == mat2->rows, MATRIX_INVALID_MULTIPLICATION);

    size_t res_rows = mat1->rows;
    size_t res_columns = mat2->columns;
    unsigned char vertical_matrix = res_rows > res_columns;
    size_t pivot = res_rows / 2;

    Matrix* result = Matrix_new_null(mat1->rows, mat2->columns);

#if defined PARALLEL_MULTIPLICATION
    pthread_t t1;
    pthread_t t2;
#endif

    Partial_Multiplication_Args args1;
    args1.result = result;
    args1.mat1 = mat1;
    args1.mat2 = mat2;
    args1.pivot = pivot;
    args1.firstPartition = 1;

    Partial_Multiplication_Args args2;
    args2.result = result;
    args2.mat1 = mat1;
    args2.mat2 = mat2;
    args2.pivot = pivot;
    args2.firstPartition = 0;

#if defined PARALLEL_MULTIPLICATION
    pthread_create(&t1, NULL, partial_mul_Matrix, &args1);
    pthread_create(&t2, NULL, partial_mul_Matrix, &args2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
#elif defined TRANSPOSED_MATRIX_HELP
    Matrix* tmp = Matrix_transpose(mat2);
    for (size_t i = 0; i < mat1->rows; ++i)
    {
        //if(i % 10 == 0 && firstPartition == 1)
        //    printf("Here starts a new multiple of 10 i index: %zu\n", i);
        for (size_t j = 0; j < mat2->columns; ++j)
        {
            //if (j % 10 == 0 && firstPartition == 1)
            //    printf("Here starts a new multiple of j index: %zu\n", j);
            MAT_T sum = 0;
            for (size_t k = 0; k < mat2->rows; ++k)
            {
                //if (k % 10 == 0 && firstPartition == 1)
                //    printf("k: %zu\n", k);
                MAT_T elemMat1 = Matrix_get(mat1, M_Pos(i, k));
                MAT_T elemMat2 = Matrix_get(tmp, M_Pos(j, k));
                sum += elemMat1 * elemMat2;
            }

            Matrix_set(result, M_Pos(i, j), sum);
        }
    }
#elif defined ADVANTAGING_WITH_CACHE_LINES
    //for (size_t i = 0; i < mat1->rows; i += SM)
    //{
    //    for (size_t j = 0; j < mat2->columns; j += SM)
    //    {
    //        for (size_t k = 0; k < mat2->rows; k += SM)
    //        {
    //            Array* rres = Matrix_get_row(result, i);
    //            Array* rmul1 = Matrix_get_row(mat1, i);
    //        }
    //    }
    //}
#else
    partial_mul_Matrix(&args1);
    partial_mul_Matrix(&args2);
#endif

    return result;
}

Matrix* Matrix_transpose(Matrix* mat)
{
    ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);

    Matrix* transposed = Matrix_new_null(mat->columns, mat->rows);

    for (size_t i = 0; i < mat->rows; ++i)
    {
        for (size_t j = 0; j < mat->columns; ++j)
        {
            MAT_T element = Matrix_get(mat, M_Pos(i, j));
            Matrix_set(transposed, M_Pos(j, i), element);
        }
    }

    return transposed;
}

void Matrix_show(Matrix* mat)
{
    ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);

    for (size_t i = 0; i < mat->rows; i++)
    {
        for (size_t j = 0; j < mat->columns; j++)
        {
            printf("%lf", Matrix_get(mat, M_Pos(i, j)));

            if (j != mat->columns - 1)
                printf("\t");
        }
        printf("\n");
    }

    return;
}
