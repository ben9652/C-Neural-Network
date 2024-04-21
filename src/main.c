#include <stdio.h>
#include <string.h>
#include "generic_management.h"
#include "NeuronsLayer.h"
#include "ActivationFunction.h"
#include "malloc.h"
#include "Matrix.h"
#include "Vector.h"
#include <stdlib.h>
#include <time.h>
#include "time_measurement.h"
#include "data_management.h"

#define TIME_MEASURED

const char* path = "data/";

float getMatrixMultiplicationTime(Matrix* m1, Matrix* m2);
float getMatrixVectorProductTime(Matrix* mat, Vector* vec);

int main()
{
    DataManagement* data = DataManagement_new("data/");

    double min = -5.0;
    double max =  5.0;

    Matrix* m = Matrix_new_random(REAL_MATRIX, &min, &max, 2, 4);
    Vector* v = Vector_new_random(REAL_VECTOR, &min, &max, 10);

    DataManagement_set_data(data, m, v);

    DataManagement_writeMatVec(data, "inputDataPrueba.txt");

    //printf("Time taken for matrices multiplication: %f miliseconds\n\n", getMatrixVectorProductTime(m, v));
    //fgetc(stdin);

    Matrix_delete(m);
    Vector_delete(v);

    return 0;
}

float getMatrixMultiplicationTime(Matrix* m1, Matrix* m2)
{
    init_time_measurement();

    Matrix_mul(m1, m2);

    finalize_time_measurement();

    return get_elapsed_time(MEASURE_MILISECONDS);
}

float getMatrixVectorProductTime(Matrix* mat, Vector* vec)
{
    init_time_measurement();

    Vector_mat_vec_product(mat, vec);

    finalize_time_measurement();

    return get_elapsed_time(MEASURE_MILISECONDS);
}