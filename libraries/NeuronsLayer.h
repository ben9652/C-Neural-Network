#ifndef __CAPA_NEURONAS_
#define __CAPA_NEURONAS_
#include "ActivationFunction.h"

typedef enum {
	INPUT_LAYER,
	OUTPUT_LAYER,
	HIDDEN_LAYER,
	NONINPUT_LAYER
} Layer_Type;

typedef struct n_layer {
	short layerNumber;
	unsigned int neuronsInLayer;

	/*
		Matriz de pesos.
		Si bien es un vector, se lo organizará para imitar a una matriz. Aún así describiremos
		este atributo como si fuese una matriz.
	
		 - El primer índice indica a qué neurona (empezando desde 0) de esta capa está conectada la arista con su respectivo peso.
		 - El segundo índice indica desde qué neurona de la capa anterior se dirige la arista, es decir, dónde tiene origen esta.
		 Por ejemplo: weightsMatrix[j][k] es el peso de la arista cuyo origen está en la k-ésima neurona de la capa anterior, y su destino en la j-ésima neurona de esta capa.
	*/
	double* weightsMatrix;

	/*
		Matriz de derivadas.
		Esta lleva las derivadas calculadas que determinarán cómo es que se deberían cambiar los pesos de la matriz de los pesos.
		Es decir que contendrá dC/dw[j][k] = (1/n)*suma(0, n-1, dC[i]/dw[j][k]).
	*/
	double* weightsGradient;

	/*
		Matriz de suma de derivadas.
		Esta lleva la suma de todas las derivadas  de los costos individuales respecto a los pesos.
		Es decir que contendrá suma(0, n-1, dC[i]/dw[j][k]).
	*/
	double* sumWeightsGradient;

	ActivationFunction actFunction;

	/*
		Entrada que tiene la capa.
		Realmente serían las activaciones de la capa anterior para las capas ocultas y la final, y la entrada de la red neuronal para la capa de entrada.
		Todas las activaciones de la capa anterior estarían conectadas a cada una de las neuronas de esta capa.
		En el caso en que esta sea la capa de entrada, cada una de las entradas corresponderían a cada una de las neuronas. Es decir que cada neurona de la capa de entrada solo tiene UNA entrada.
	*/
	double* input;

	/*
		Es el vector de salida de las neuronas.
		Cada uno de los elementos corresponde a la salida de cada una de las neuronas.
		Por ejemplo: output[i] es la salida de la i-ésima neurona que hay en la capa.
	*/
	double* output;

	/* Son los sesgos que tiene cada una de las neuronas. */
	double* biases;
	double* biasesGradient;
	double* sumBiasesGradient;

	/*
		Número de entradas de la capa.
		Si esta es la capa de entrada, sería igual a la cantidad de neuronas que hay en esta capa. En otro caso, sería el número de activaciones que le llega a cada neurona de esta capa.
	*/
	unsigned int inputsNumber;
} NeuronsLayer;

NeuronsLayer* new_NeuronsLayer1(Layer_Type type, unsigned int inputsNumber);
NeuronsLayer* new_NeuronsLayer2(Layer_Type type, unsigned int neuronsInLayer, ActivationFunction af, unsigned int inputsNumber);
NeuronsLayer* new_NeuronsLayer3(Layer_Type type, unsigned int neuronsInLayer, ActivationFunction af, unsigned int inputsNumber, double* weights);
NeuronsLayer* new_NeuronsLayer4(Layer_Type type, unsigned int neuronsInLayer, ActivationFunction af, unsigned int inputsNumber, double* biases);
NeuronsLayer* new_NeuronsLayer5(Layer_Type type, unsigned int neuronsInLayer, ActivationFunction af, unsigned int inputsNumber, double* weights, double* biases);



#endif