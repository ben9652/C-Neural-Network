#include "Layer.h"
#include <stdlib.h>
#include "error_management.h"
#include "Random.h"
#include "DetectMemoryLeaks.h"

/* Funciones privadas */

Layer* constructor_base(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber);
Vector applyFunctionToVector(ActivationFunction* af, Vector* vector);

Layer* Layer_new_input(size_t inputsNumber)
{
	Layer* newLayer = NULL;

	// Reservo espacio de memoria para esta capa
	newLayer = (Layer*)malloc(sizeof(Layer));
	ASSERT(newLayer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newLayer->layerType = INPUT_LAYER;
	newLayer->layerNumber = 0;
	newLayer->inputsNumber = inputsNumber;
	newLayer->neuronsInLayer = inputsNumber;
	newLayer->prevLayer = NULL;
	newLayer->nextLayer = NULL;
	newLayer->input = Vector_new(inputsNumber);
	newLayer->output = Vector_new(inputsNumber);
	newLayer->outputBeforeActivation = Vector_new_zero(inputsNumber);
	newLayer->actFunction = ActivationFunction_new(SIGMOID, 1, 0);

	newLayer->weights = NULL;
	newLayer->weightsGradient = NULL;
	newLayer->sumWeightsGradient = NULL;

	newLayer->biases = NULL;
	newLayer->biasesGradient = NULL;
	newLayer->sumBiasesGradient = NULL;

	return newLayer;
}

Layer* Layer_new_1(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber)
{
	Layer* newLayer = constructor_base(type, neuronsInLayer, af, inputsNumber);

	MAT_T min = -1.0;
	MAT_T max =  1.0;
	MAT_T zero = 0.0;
	newLayer->weights = Matrix_new_random(REAL_MATRIX, &min, &max, neuronsInLayer, inputsNumber);
	newLayer->biases = Vector_new_random(REAL_MATRIX, &min, &zero, neuronsInLayer);

	return newLayer;
}

Layer* Layer_new_2(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber, Matrix* weights)
{
	Layer* newLayer = constructor_base(type, neuronsInLayer, af, inputsNumber);

	MAT_T min = -1.0;
	MAT_T zero = 0.0;

	newLayer->weights = Matrix_new_copy(weights);
	Matrix_delete(weights);

	newLayer->biases = Vector_new_random(REAL_MATRIX, &min, &zero, inputsNumber);

	return newLayer;
}

Layer* Layer_new_3(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber, Vector* biases)
{
	Layer* newLayer = constructor_base(type, neuronsInLayer, af, inputsNumber);

	MAT_T min = -1.0;
	MAT_T max =  1.0;
	newLayer->weights = Matrix_new_random(REAL_MATRIX, &min, &max, neuronsInLayer, inputsNumber);
	newLayer->biases = Vector_new_copy(biases);

	return newLayer;
}

Layer* Layer_new_4(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber, Matrix* weights, Vector* biases)
{
	Layer* newLayer = constructor_base(type, neuronsInLayer, af, inputsNumber);
	newLayer->weights = Matrix_new_copy(weights);
	newLayer->biases = Vector_new_copy(biases);

	return newLayer;
}

void Layer_delete(Layer* self)
{
	ActivationFunction_delete(self->actFunction);

	Matrix_delete(self->weights);
	Matrix_delete(self->weightsGradient);
	Matrix_delete(self->sumWeightsGradient);

	Vector_delete(self->input);
	Vector_delete(self->output);
	Vector_delete(self->outputBeforeActivation);

	Vector_delete(self->biases);
	Vector_delete(self->biasesGradient);
	Vector_delete(self->sumBiasesGradient);

	free(self);
}

void Layer_outputCalculation(Layer* self)
{
	ASSERT(self->inputsNumber, N_LAYER_WITHOUT_INPUT);

	ActivationFunction* actFn = self->actFunction;

	Vector fnAppliedToOutput;
	//Vector_delete(self->output);
	if (self->layerType != INPUT_LAYER)
	{
		// Se hace WX
		Vector WX = Vector_mat_vec_product_stacked(self->weights, self->input);

		// Se hace WX+B
		Vector outputBeforeActivation = Vector_sum_stacked(&WX, self->biases);
		Vector_set_another_vector(self->outputBeforeActivation, &outputBeforeActivation);

		// Finalmente se aplica la función de activación, dando la salida total de esta capa
		fnAppliedToOutput = applyFunctionToVector(actFn, self->outputBeforeActivation);

		Vector_delete_stacked(&outputBeforeActivation);
		Vector_delete_stacked(&WX);
	}
	else
		fnAppliedToOutput = applyFunctionToVector(actFn, self->input);

	Vector_move_stacked(self->output, &fnAppliedToOutput);
}

void Layer_learn(Layer* self, struct bp_s* bp, double learningRate)
{
	// Se actualizan los gradientes en base a las activaciones en las salidas de todas las neuronas
	updateParameters(bp, self);

	Matrix* weightsGradWithLearningRate = Matrix_product_with_scalar(self->weightsGradient, -learningRate);
	Matrix_sum_void(self->weights, weightsGradWithLearningRate);

	Vector* biasesGradWithLearningRate = Vector_product_with_scalar(self->biasesGradient, -learningRate);
	Vector_sum_void(self->biases, biasesGradWithLearningRate);

	Matrix_delete(weightsGradWithLearningRate);
	Vector_delete(biasesGradWithLearningRate);
}

Vector applyFunctionToVector(ActivationFunction* af, Vector* vector)
{
	Vector appliedFunction = Vector_create(vector->size);

	for (size_t i = 0; i < vector->size; i++)
	{
		VEC_T computedFunction = calc(af, Vector_get(vector, i));
		Vector_set(&appliedFunction, i, computedFunction);
	}
	
	return appliedFunction;
}

Layer* constructor_base(Layer_Type type, size_t neuronsInLayer, ActivationFunction* af, size_t inputsNumber)
{
	ASSERT(type != INPUT_LAYER, N_LAYER_CANT_BE_INPUT);
	Layer* newLayer = NULL;

	newLayer = (Layer*)malloc(sizeof(Layer));
	ASSERT(newLayer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newLayer->layerType = type;
	newLayer->inputsNumber = inputsNumber;
	newLayer->neuronsInLayer = neuronsInLayer;

	newLayer->prevLayer = NULL;
	newLayer->nextLayer = NULL;

	newLayer->weightsGradient = Matrix_new(neuronsInLayer, inputsNumber);
	newLayer->sumWeightsGradient = Matrix_new_zero(neuronsInLayer, inputsNumber);

	newLayer->biasesGradient = Vector_new(neuronsInLayer);
	newLayer->sumBiasesGradient = Vector_new_zero(neuronsInLayer);

	newLayer->actFunction = af;
	newLayer->input = Vector_new(inputsNumber);
	newLayer->output = Vector_new(neuronsInLayer);
	newLayer->outputBeforeActivation = Vector_new_zero(neuronsInLayer);

	return newLayer;
}