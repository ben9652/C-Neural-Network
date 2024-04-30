#include "Matrix.h"
#include "error_management.h"
#include <string.h>
#include <stdlib.h>
#include "pthread.h"
#include "Random.h"
#include "DetectMemoryLeaks.h"

//#define PARALLEL_MULTIPLICATION
//#define TRANSPOSED_MATRIX_HELP
#define ADVANTAGING_WITH_CACHE_LINES

#define NUM_THREADS 8

#define WITH_THREADS

typedef struct {
    Matrix* result;
    Matrix* mat1;
    Matrix* mat2;
    size_t pivot;
    unsigned char firstPartition;
} Partial_Multiplication_Args;

typedef struct {
    Matrix* result;
    Matrix* mat1;
    Matrix* mat2;
    size_t start;
    size_t end;
} Partial_Sum_Args;

void* partial_mul_Matrix(Partial_Multiplication_Args* args);
void* matrix_sum_thread(Partial_Sum_Args* args);
void matrix_sum_traditional(Matrix* res, Matrix* mat1, Matrix* mat2);

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

Matrix* Matrix_new_zero(size_t rows, size_t columns)
{
    size_t dim = rows * columns;
    Matrix* matrix = Matrix_new(rows, columns);
    ASSERT(matrix, MATRIX_NOT_CREATED_EXCEPTION);

    memset(matrix->Buffer, 0, dim * sizeof(MAT_T));

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

Matrix Matrix_create_random(Matrix_Datatype datatype, void* min, void* max, size_t rows, size_t columns)
{
    size_t sizeMatrix = rows * columns * sizeof(MAT_T);

    Matrix mat;
    Matrix* matrix = &mat;

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

    return *matrix;
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
    if (matrix != NULL)
    {
        if(matrix->Buffer != NULL)
            free(matrix->Buffer);
        free(matrix);
    }
    else
        free(matrix);

    matrix = NULL;
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

void* matrix_sum_thread(Partial_Sum_Args* args) {

    MAT_T* mat1 = args->mat1->Buffer;
    MAT_T* mat2 = args->mat2->Buffer;
    MAT_T* res = args->result->Buffer;
    size_t start = args->start;
    size_t end = args->end;

    // Suma de las matrices
    for (size_t i = start; i < end; i++)
        *(res + i) = *(mat1 + i) + *(mat2 + i);

    pthread_exit(NULL);

    return NULL;
}

void matrix_sum_traditional(Matrix* res, Matrix* mat1, Matrix* mat2)
{
    MAT_T* buff1 = mat1->Buffer;
    MAT_T* buff2 = mat2->Buffer;
    MAT_T* buffRes = res->Buffer;

    size_t sizeOfMatrices = mat1->rows * mat2->columns;

    for (size_t i = 0; i < sizeOfMatrices; i++)
        *(buffRes + i) = *(buff1 + i) + *(buff2 + i);
}

Matrix* Matrix_sum(Matrix* mat1, Matrix* mat2)
{
    ASSERT(mat1, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat2, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat1->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(mat2->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(mat1->rows == mat2->rows && mat1->columns == mat2->columns, MATRIX_INVALID_ADDITION);

    size_t rows = mat1->rows;
    size_t columns = mat1->columns;
    size_t sizeOfMatrices = rows * columns;

    Matrix* res = Matrix_new_null(rows, columns);

#if defined WITH_THREADS

    size_t elements_per_thread;

    // La suma en paralelo solo vale la pena cuando hay una cantidad de datos gigante en la matriz
    if (sizeOfMatrices > 25e6)
        elements_per_thread = 0;
    else
        elements_per_thread = sizeOfMatrices / NUM_THREADS;

    if (elements_per_thread)
    {
        pthread_t threads[NUM_THREADS];
        Partial_Sum_Args thread_args[NUM_THREADS];

        for (size_t i = 0; i < NUM_THREADS; i++)
        {
            size_t start = i * elements_per_thread;
            size_t end = (i == NUM_THREADS - 1) ? sizeOfMatrices : (i + 1) * elements_per_thread;

            thread_args[i].start = start;
            thread_args[i].end = end;
            thread_args[i].mat1 = mat1;
            thread_args[i].mat2 = mat2;
            thread_args[i].result = res;

            pthread_create(&threads[i], NULL, matrix_sum_thread, (void*)&thread_args[i]);
        }

        for (size_t i = 0; i < NUM_THREADS; i++)
            pthread_join(threads[i], NULL);
    }
    else
        matrix_sum_traditional(res, mat1, mat2);

#else

    matrix_sum_traditional(res, mat1, mat2);

#endif

    return res;
}

void Matrix_sum_void(Matrix* mat1, Matrix* mat2)
{
    ASSERT(mat1, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat2, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat1->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(mat2->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(mat1->rows == mat2->rows && mat1->columns == mat2->columns, MATRIX_INVALID_ADDITION);

#if defined WITH_THREADS

    size_t rows = mat1->rows;
    size_t columns = mat1->columns;
    size_t sizeOfMatrices = rows * columns;

    pthread_t threads[NUM_THREADS];
    Partial_Sum_Args thread_args[NUM_THREADS];

    size_t elements_per_thread;

    // La suma en paralelo solo vale la pena cuando hay una cantidad de datos gigante en la matriz
    if (sizeOfMatrices > 25e6)
        elements_per_thread = 0;
    else
        elements_per_thread = sizeOfMatrices / NUM_THREADS;

    if (elements_per_thread)
    {
        for (size_t i = 0; i < NUM_THREADS; i++)
        {
            size_t start = i * elements_per_thread;
            size_t end = (i == NUM_THREADS - 1) ? sizeOfMatrices : (i + 1) * elements_per_thread;

            thread_args[i].start = start;
            thread_args[i].end = end;
            thread_args[i].mat1 = mat1;
            thread_args[i].mat2 = mat2;
            thread_args[i].result = mat1;

            pthread_create(&threads[i], NULL, matrix_sum_thread, (void*)&thread_args);
        }

        for (size_t i = 0; i < NUM_THREADS; i++)
            pthread_join(threads[i], NULL);
    }
    else
        matrix_sum_traditional(mat1, mat1, mat2);

#else

    matrix_sum_traditional(mat1, mat1, mat2);

#endif
}

MAT_T Matrix_sum_element(Matrix* mat, Matrix_Position pos, MAT_T element)
{
    ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);

    size_t index = pos.j + pos.i * mat->columns;
    mat->Buffer[index] += element;
    return mat->Buffer[index];
}

void Matrix_set_row(Matrix* matrix, size_t row_index, MAT_T* row, size_t size)
{
    ASSERT(matrix, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(row, MEMORY_POINTING_TO_NOTHING);
    ASSERT(row_index < matrix->rows, MATRIX_OUT_OF_BOUNDS_EXCEPTION);
    ASSERT(size <= matrix->columns, MATRIX_OUT_OF_BOUNDS_EXCEPTION);

    memmove(matrix->Buffer + row_index, row, size * sizeof(MAT_T));
}

void Matrix_set_another_matrix(Matrix* dest, Matrix* src)
{
    ASSERT(dest, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(src, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(dest->Buffer, MEMORY_POINTING_TO_NOTHING);
    ASSERT(src->Buffer, MEMORY_POINTING_TO_NOTHING);

    dest->rows = src->rows;
    dest->columns = src->columns;

    if (dest->rows != src->rows || dest->columns != src->columns)
    {
        MAT_T* tmp = NULL;
        while(tmp == NULL)
            tmp = (MAT_T*)realloc(dest->Buffer, src->rows * src->columns);
        dest->Buffer = tmp;
    }

    memmove(dest->Buffer, src->Buffer, src->rows * src->columns);
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
#else
    partial_mul_Matrix(&args1);
    partial_mul_Matrix(&args2);
#endif

    return result;
}

Matrix* Matrix_product_with_scalar(Matrix* mat, MAT_T scalar)
{
    ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);

    Matrix* res = Matrix_new_copy(mat);

    for (size_t i = 0; i < mat->rows; i++)
    {
        for (size_t j = 0; j < mat->columns; j++)
        {
            MAT_T product = Matrix_get(mat, M_Pos(i, j)) * scalar;
            Matrix_set(res, M_Pos(i, j), product);
        }
    }

    return res;
}

void Matrix_product_with_scalar_void(Matrix* mat, MAT_T scalar)
{
    ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
    ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);

    for (size_t i = 0; i < mat->rows; i++)
    {
        for (size_t j = 0; j < mat->columns; j++)
        {
            MAT_T product = Matrix_get(mat, M_Pos(i, j)) * scalar;
            Matrix_set(mat, M_Pos(i, j), product);
        }
    }
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
