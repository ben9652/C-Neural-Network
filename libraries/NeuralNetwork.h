#ifndef __NEURAL_NETWORK
#define __NEURAL_NETWORK

#include "Layer.h"
#include "Backpropagation.h"
#include "Vector.h"
#include "Matrix.h"
#include "data_management.h"
#include "ActivationFunction.h"
#include "VectorPointers.h"

typedef struct
{
	char* name;
	Layer* inputLayer;
	VectorPointers* hiddenLayers;
	Layer* lastLayer;
	Layer* outputLayer;
	Vector* input;
	Vector* output;
	double performance;
	size_t successfulTests;
	size_t qntyTests;
	double tolerance;
	Backpropagation* algorithm;
	double averageCost;
	double totalCost;

	DataManagement* parameters_management;
} NeuralNetwork;

NeuralNetwork* NeuralNetwork_new(const char* name, size_t inputsNumber, size_t outputsNumber, Vector* hiddenNeuronsNumber, VectorPointers* hiddenActFn, ActivationFunction* outputActFn, unsigned char extract_from_file);

void NeuralNetwork_delete(NeuralNetwork* nn);

void NeuralNetwork_learn(NeuralNetwork* nn, Vector* input, Vector* desiredOutput, double learningRate, unsigned char writeResult);
void NeuralNetwork_test(NeuralNetwork* nn, Vector* input, Vector* desiredOutput, unsigned char writeResult);

#endif