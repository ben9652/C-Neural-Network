#include "Backpropagation.h"
#include <stdlib.h>
#include <math.h>

double dC_dw(size_t j, size_t k, NeuronsLayer* layer);
double dC_db(size_t j, NeuronsLayer* layer);
double dC_da(size_t neuron, NeuronsLayer* layer);

Backpropagation* new_Backpropagation()
{
    return NULL;
}

double updateCost(double* output, double* desiredOutput, double totalCost)
{
    double iterationCost = 0.0;

    return totalCost;
}

double* updateParameters(double* weightsGradientMatrix, double* sumWeightsGradientMatrix, double* biasesGradient, double* sumBiasesGradient)
{
    return NULL;
}
