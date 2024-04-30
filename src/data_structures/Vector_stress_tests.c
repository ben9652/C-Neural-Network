#include "Vector_stress_tests.h"
#include "Vector.h"

float Vector_new_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests)
{
    init_time_measurement();

    double min = 0.0;
    double max = 1.0;

    for (size_t i = 0; i < qntyTests; i++)
    {
        Vector* vHeap = Vector_new_random(REAL_VECTOR, &min, &max, sizeOfVec);

        Vector_delete(vHeap);
    }

    finalize_time_measurement();

    return get_elapsed_time(unit);
}

float Vector_new_move_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests)
{
    init_time_measurement();

    double min = 0.0;
    double max = 1.0;

    for (size_t i = 0; i < qntyTests; i++)
    {
        Vector* vToMove = Vector_new_random(REAL_VECTOR, &min, &max, sizeOfVec);

        Vector* vHeap = Vector_new_move(vToMove);

        Vector_delete(vHeap);
    }

    finalize_time_measurement();

    return get_elapsed_time(unit);
}

float Vector_mat_vec_product_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests)
{
    init_time_measurement();

    double min = 0.0;
    double max = 1.0;

    Matrix* mat = Matrix_new_random(REAL_MATRIX, &min, &max, sizeOfVec, sizeOfVec);
    Vector* v = Vector_new_random(REAL_VECTOR, &min, &max, sizeOfVec);

    for (size_t i = 0; i < qntyTests; i++)
    {
        Vector* vHeap = Vector_mat_vec_product(mat, v);

        Vector_delete(vHeap);
    }

    Matrix_delete(mat);
    Vector_delete(v);

    finalize_time_measurement();

    return get_elapsed_time(unit);
}

float Vector_create_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests)
{
    init_time_measurement();

    double min = 0.0;
    double max = 1.0;

    for (size_t i = 0; i < qntyTests; i++)
    {
        Vector vStack = Vector_create_random(REAL_VECTOR, &min, &max, sizeOfVec);

        Vector_delete_stacked(&vStack);
    }

    finalize_time_measurement();

    return get_elapsed_time(unit);
}

float Vector_create_move_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests)
{
    init_time_measurement();

    double min = 0.0;
    double max = 1.0;

    for (size_t i = 0; i < qntyTests; i++)
    {
        Vector vToMove = Vector_create_random(REAL_VECTOR, &min, &max, sizeOfVec);
        
        Vector vStack = Vector_create_move(&vToMove);

        Vector_delete(&vStack);
    }

    finalize_time_measurement();

    return get_elapsed_time(unit);
}

float Vector_mat_vec_product_stacked_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests)
{
    init_time_measurement();

    double min = 0.0;
    double max = 1.0;

    Matrix* mat = Matrix_new_random(REAL_MATRIX, &min, &max, sizeOfVec, sizeOfVec);
    Vector v = Vector_create_random(REAL_VECTOR, &min, &max, sizeOfVec);

    for (size_t i = 0; i < qntyTests; i++)
    {
        Vector vStack = Vector_mat_vec_product_stacked(mat, &v);

        Vector_delete_stacked(&vStack);
    }

    Matrix_delete(mat);
    Vector_delete_stacked(&v);

    finalize_time_measurement();

    return get_elapsed_time(unit);
}
