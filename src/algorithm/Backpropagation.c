#include "Backpropagation.h"
#include <stdlib.h>
#include <math.h>
#include "error_management.h"

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
