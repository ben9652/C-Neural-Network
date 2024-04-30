#ifndef __CAPA_NEURONAS_
#define __CAPA_NEURONAS_
#include "ActivationFunction.h"
#include "Matrix.h"
#include "Vector.h"
#include "Backpropagation.h"

struct bp_s;

typedef enum {
	INPUT_LAYER,
	OUTPUT_LAYER,
	HIDDEN_LAYER,
	NONINPUT_LAYER
} Layer_Type;

typedef struct n_layer {
	Layer_Type layerType;

	short layerNumber;
	size_t neuronsInLayer;

	/*
		Matriz de pesos.
	
		 - El primer índice indica a qué neurona (empezando desde 0) de esta capa está conectada la arista con su respectivo peso.
		 - El segundo índice indica desde qué neurona de la capa anterior se dirige la arista, es decir, dónde tiene origen esta.
		 Por ejemplo: weightsMatrix[j][k] es el peso de la arista cuyo origen está en la k-ésima neurona de la capa anterior, y su destino en la j-ésima neurona de esta capa.
	*/
	Matrix* weights;

	/*
		Matriz de derivadas.
		Esta lleva las derivadas calculadas que determinarán cómo es que se deberían cambiar los pesos de la matriz de los pesos.
		Es decir que contendrá dC/dw[j][k] = (1/n)*suma(0, n-1, dC[i]/dw[j][k]).
	*/
	Matrix* weightsGradient;

	/*
		Matriz de suma de derivadas.
		Esta lleva la suma de todas las derivadas  de los costos individuales respecto a los pesos.
		Es decir que contendrá suma(0, n-1, dC[i]/dw[j][k]).
	*/
	Matrix* sumWeightsGradient;

	ActivationFunction* actFunction;

	struct n_layer* prevLayer;
	struct n_layer* nextLayer;

	/*
		Entrada que tiene la capa.
		Realmente serían las activaciones de la capa anterior para las capas ocultas y la final, y la entrada de la red neuronal para la capa de entrada.
		Todas las activaciones de la capa anterior estarían conectadas a cada una de las neuronas de esta capa.
		En el caso en que esta sea la capa de entrada, cada una de las entradas corresponderían a cada una de las neuronas. Es decir que cada neurona de la capa de entrada solo tiene UNA entrada.
	*/
	Vector* input;

	/*
		Es el vector de salida de las neuronas.
		Cada uno de los elementos corresponde a la salida de cada una de las neuronas.
		Por ejemplo: output[i] es la salida de la i-ésima neurona que hay en la capa.
	*/
	Vector* output;

	/*
		Es la salida que ofrece cada una de las neuronas antes de activarse. Es decir,
		es la suma ponderada de las entradas multiplicadas por los pesos, más el bias del final.
	*/
	Vector* outputBeforeActivation;

	/* Son los sesgos que tiene cada una de las neuronas. */
	Vector* biases;
	Vector* biasesGradient;
	Vector* sumBiasesGradient;
	// El tamaño de estos vectores es el mismo que el de neuronsInLayer

	/*
		Número de entradas de la capa.
		Si esta es la capa de entrada, sería igual a la cantidad de neuronas que hay en esta capa. En otro caso, sería el número de activaciones que le llega a cada neurona de esta capa.
	*/
	size_t inputsNumber;
} Layer;

/// <summary>
/// Capa de neuronas nueva. Esta es la capa de entrada.
/// </summary>
/// <param name="inputsNumber">Número de entradas. En este caso, el número de entradas corresponde al número de neuronas de esta capa.</param>
/// <returns>Una capa de neuronas nueva</returns>
Layer* Layer_new_input(size_t inputsNumber);

/// <summary>
/// Capa de neuronas nueva. Se usa cuando se quiere especificar el número de neuronas en ella y la función de activación de salida en cada una de estas.
/// </summary>
/// <param name="type">Tipo de capa: entrada, oculta, o de salida</param>
/// <param name="neuronsInLayer">Número de neuronas presentes en la capa</param>
/// <param name="af">Función de activación presente en todas las neuronas de esta capa</param>
/// <param name="inputsNumber">Número de entradas de cada neurona (la salida de la capa anterior)</param>
/// <returns>Una capa de neuronas nueva</returns>
Layer* Layer_new_1(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber);

/// <summary>
/// Capa de neuronas nueva. Se usa cuando se quiere especificar las características de una capa, y se quiere asignar manualmente los pesos.
/// </summary>
/// <param name="type">Tipo de capa: entrada, oculta, o de salida</param>
/// <param name="neuronsInLayer">Número de neuronas presentes en la capa</param>
/// <param name="af">Función de activación presente en todas las neuronas de esta capa</param>
/// <param name="inputsNumber">Número de entradas de cada neurona (la salida de la capa anterior)</param>
/// <param name="weights">Matriz de pesos correspondiente a esta capa</param>
/// <returns>Una capa de neuronas nueva</returns>
Layer* Layer_new_2(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber, Matrix* weights);

/// <summary>
/// Capa de neuronas nueva. Se usa cuando se quiere especificar las características de una capa, y se quiere asignar manualmente los biases.
/// </summary>
/// <param name="type">Tipo de capa: entrada, oculta, o de salida</param>
/// <param name="neuronsInLayer">Número de neuronas presentes en la capa</param>
/// <param name="af">Función de activación presente en todas las neuronas de esta capa</param>
/// <param name="inputsNumber">Número de entradas de cada neurona (la salida de la capa anterior)</param>
/// <param name="biases">Vector de sesgos (o biases) que representa al bias que tiene cada neurona de la capa</param>
/// <returns>Una capa de neuronas nueva</returns>
Layer* Layer_new_3(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber, Vector* biases);

/// <summary>
/// Capa de neuronas nueva. Se usa cuando se quiere especificar las características de una capa, y se quiere asignar manualmente los pesos y biases.
/// </summary>
/// <param name="type">Tipo de capa: entrada, oculta, o de salida</param>
/// <param name="neuronsInLayer">Número de neuronas presentes en la capa</param>
/// <param name="af">Función de activación presente en todas las neuronas de esta capa</param>
/// <param name="inputsNumber">Número de entradas de cada neurona (la salida de la capa anterior)</param>
/// <param name="weights">Matriz de pesos correspondiente a esta capa</param>
/// <param name="biases">Vector de sesgos (o biases) que representa al bias que tiene cada neurona de la capa</param>
/// <returns>Una capa de neuronas nueva</returns>
Layer* Layer_new_4(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber, Matrix* weights, Vector* biases);

void Layer_delete(Layer* self);

void Layer_outputCalculation(Layer* self);

void Layer_learn(Layer* self, struct bp_s* bp, double learningRate);

#endif