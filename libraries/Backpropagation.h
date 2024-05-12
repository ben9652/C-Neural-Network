#ifndef __BACKPROPAGATION__
#define __BACKPROPAGATION__
#include "Layer.h"

struct n_layer;

// Esta estructura lleva fundamentalmente la cuenta de las iteraciones que se va haciendo en el aprendizaje y la salida deseada (al ser una red supervisada)
typedef struct bp_s {
	unsigned int iteration;
	Vector* desiredOutput;
} Backpropagation;

typedef struct {
	Matrix* previousWeigthsGradient;
} PreviousGradients;

Backpropagation* Backpropagation_new(size_t outputSize);

void Backpropagation_delete(Backpropagation* self);

double updateCost(Backpropagation* bp, Vector* output, Vector* desiredOutput, double totalCost);

#endif
