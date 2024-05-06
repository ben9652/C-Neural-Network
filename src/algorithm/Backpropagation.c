#include "Backpropagation.h"
#include <stdlib.h>
#include <math.h>
#include "error_management.h"

double dC_dw(Backpropagation* bp, double derivativeCostOverAct, size_t j, size_t k, Layer* layer);
double dC_db(Backpropagation* bp, double derivativeCostOverAct, size_t j, Layer* layer);
double dC_da(Backpropagation* bp, size_t neuron, Layer* layer);

Backpropagation* Backpropagation_new(size_t outputSize)
{
    Backpropagation* bp = (Backpropagation*)malloc(sizeof(Backpropagation));
    ASSERT(bp, MEMORY_NOT_ASSIGNED_EXCEPTION);
    bp->iteration = 0;
    bp->desiredOutput = Vector_new(outputSize);

    return bp;
}

void Backpropagation_delete(Backpropagation* self)
{
    Vector_delete(self->desiredOutput);
    free(self);
}

double updateCost(Backpropagation* bp, Vector* output, Vector* desiredOutput, double totalCost)
{
    double costTilThisIteration = 0.0;
    for (size_t i = 0; i < output->size; i++)
        costTilThisIteration += pow(Vector_get(output, i) - Vector_get(desiredOutput, i), 2);
    bp->iteration++;
    Vector_set_another_vector(bp->desiredOutput, desiredOutput);

    totalCost += (1.0 / output->size) * costTilThisIteration;

    return totalCost;
}

void updateParameters(Backpropagation* bp, struct n_layer* layer)
{
    Vector gradientCostOverAct = Vector_create_zero(layer->neuronsInLayer);

    VEC_T* buffer = gradientCostOverAct.Buffer;

    for (size_t i = 0; i < gradientCostOverAct.size; i++)
        *(buffer + i) = dC_da(bp, i, layer);

    Matrix* weightsGrad = layer->weightsGradient;
    Matrix* sumWeightsGrad = layer->sumWeightsGradient;

    Vector* biasesGrad = layer->biasesGradient;
    Vector* sumBiasesGrad = layer->sumBiasesGradient;

    for (size_t j = 0; j < weightsGrad->rows; j++)
    {
        double DerivativeCostOverActivation = Vector_get(&gradientCostOverAct, j);
        double calculateAverage = 1.0 / bp->iteration;

        VEC_T DerivativeCostOverBias = dC_db(bp, DerivativeCostOverActivation, j, layer);
        VEC_T addedBiasDerivative = Vector_sum_element(sumBiasesGrad, j, DerivativeCostOverBias);
        Vector_set(biasesGrad, j, calculateAverage * addedBiasDerivative);

        for (size_t k = 0; k < weightsGrad->columns; k++)
        {
            MAT_T DerivativeCostOverWeight = dC_dw(bp, DerivativeCostOverActivation, j, k, layer);
            Matrix_Position pos = M_Pos(j, k);

            MAT_T addedWeightDerivative = Matrix_sum_element(sumWeightsGrad, pos, DerivativeCostOverWeight);
            Matrix_set(weightsGrad, pos, calculateAverage * addedWeightDerivative);
        }
    }

    Vector_delete_stacked(&gradientCostOverAct);
}

static double dC_dw(Backpropagation* bp, double derivativeCostOverAct, size_t j, size_t k, Layer* layer)
{
    double outputActivation = Vector_get(layer->outputBeforeActivation, j);
    double outputPreviousLayer = Vector_get(layer->prevLayer->output, k);

    return
        derivativeCostOverAct *
        deriv(layer->actFunction, outputActivation) *
        outputPreviousLayer
        ;
}

static double dC_db(Backpropagation* bp, double derivativeCostOverAct, size_t j, Layer* layer)
{
    double outputActivation = Vector_get(layer->outputBeforeActivation, j);

    return
        derivativeCostOverAct *
        deriv(layer->actFunction, outputActivation)
        ;
}

static double dC_da(Backpropagation* bp, size_t neuron, Layer* layer)
{
    VEC_T actualOutput = Vector_get(layer->output, neuron);

    // Solo se usará esta variable cuando la capa a la que se hace referencia es la de salida
    VEC_T desiredOutput = 0.0;
    if (layer->layerType == OUTPUT_LAYER)
        desiredOutput = Vector_get(bp->desiredOutput, neuron);

    double diffBetweenOutputAndDesiredOutput = actualOutput - desiredOutput;
    double applyAverage = 1.0 / layer->neuronsInLayer;
    
    if (layer->layerType == OUTPUT_LAYER)
        // La derivada del costo respecto a la activación de la última capa no es más que la derivada del error cuadrático medio. La diferencia entre la activación de la neurona de la última capa y la salida deseada en esta neurona es la que se calcula en el error cuadrático medio.
        return applyAverage * 2 * diffBetweenOutputAndDesiredOutput;
    else
    {
        Layer* next = layer->nextLayer;
        size_t neuronsNextLayer = next->neuronsInLayer;
        double sum = 0.0;
        for (size_t j_index = 0; j_index < neuronsNextLayer; j_index++)
        {
            double outputActivation = Vector_get(next->outputBeforeActivation, j_index);
            double weight = Matrix_get(next->weights, M_Pos(j_index, neuron));
            sum +=  dC_da(bp, j_index, next) *
                    deriv(next->actFunction, outputActivation) *
                    weight
                ;
        }
        return sum;
    }
}
