#ifndef __BACKPROPAGATION__
#define __BACKPROPAGATION__
#include "NeuronsLayer.h"

typedef struct {
	unsigned int iteration;
	double* desiredOutput;
	double CostOfActivation_derivative;
} Backpropagation;

Backpropagation* new_Backpropagation();

double updateCost(double* output, double* desiredOutput, double totalCost);
double* updateParameters(double* weightsGradientMatrix, double* sumWeightsGradientMatrix, double* biasesGradient, double* sumBiasesGradient);

#endif