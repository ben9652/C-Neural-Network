#ifndef __VECTOR_TESTS_
#define __VECTOR_TESTS_
#include "time_measurement.h"
#include <stdlib.h>

/************  Tests de creación en el Heap  ************/

/// <summary>
/// Evalúa la asignación de vectores en el Heap
/// </summary>
/// <param name="unit">Unidad de medida de tiempo que se quiere obtener</param>
/// <param name="sizeOfVec">Tamaño de los vectores que se quiere generar</param>
/// <param name="qntyTests">Cantidad de vectores que se quiere generar</param>
/// <returns>Tiempo de demora en la unidad especificada</returns>
float Vector_new_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests);

/// <summary>
/// Evalúa la creación de vectores en el Heap mediante movimiento de datos de uno a otro
/// </summary>
/// <param name="unit">Unidad de medida de tiempo que se quiere obtener</param>
/// <param name="sizeOfVec">Tamaño de los vectores que se quiere generar</param>
/// <param name="qntyTests">Cantidad de vectores que se quiere generar</param>
/// <returns>Tiempo de demora en la unidad especificada</returns>
float Vector_new_move_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests);

/// <summary>
/// Evalúa la creación de vectores en el Heap mediante multiplicaciones de matrices por vectores
/// </summary>
/// <param name="unit">Unidad de medida de tiempo que se quiere obtener</param>
/// <param name="sizeOfVec">Tamaño de los vectores que se quiere generar</param>
/// <param name="qntyTests">Cantidad de vectores que se quiere generar</param>
/// <returns>Tiempo de demora en la unidad especificada</returns>
float Vector_mat_vec_product_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests);


/************  Tests de creación en el Stack  ************/

/// <summary>
/// Evalúa la asignación de vectores en el Stack
/// </summary>
/// <param name="unit">Unidad de medida de tiempo que se quiere obtener</param>
/// <param name="sizeOfVec">Tamaño de los vectores que se quiere generar</param>
/// <param name="qntyTests">Cantidad de vectores que se quiere generar</param>
/// <returns>Tiempo de demora en la unidad especificada</returns>
float Vector_create_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests);

/// <summary>
/// Evalúa la creación de vectores en el Stack mediante movimiento de datos de uno a otro
/// </summary>
/// <param name="unit">Unidad de medida de tiempo que se quiere obtener</param>
/// <param name="sizeOfVec">Tamaño de los vectores que se quiere generar</param>
/// <param name="qntyTests">Cantidad de vectores que se quiere generar</param>
/// <returns>Tiempo de demora en la unidad especificada</returns>
float Vector_create_move_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests);

/// <summary>
/// Evalúa la creación de vectores en el Heap mediante multiplicaciones de matrices por vectores
/// </summary>
/// <param name="unit">Unidad de medida de tiempo que se quiere obtener</param>
/// <param name="sizeOfVec">Tamaño de los vectores que se quiere generar</param>
/// <param name="qntyTests">Cantidad de vectores que se quiere generar</param>
/// <returns>Tiempo de demora en la unidad especificada</returns>
float Vector_mat_vec_product_stacked_stress_test(Time_Unit unit, size_t sizeOfVec, size_t qntyTests);

#endif