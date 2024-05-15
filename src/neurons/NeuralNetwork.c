#include "NeuralNetwork.h"
#include "error_management.h"
#include "StringVec.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <direct.h>
#include "DetectMemoryLeaks.h"
#include "GUI.h"
#include "unordered_map.h"

#define SEPARATOR "\n\t\t-------------------- O --------------------\n\n"
//#define USE_MAP

static unsigned char firstTimeWriting = 1;
static size_t counterWritingOverflow = 0;

static unordered_map* derivativesCostOverActivation;
static int dC_da_EnteredTimes = 0;

void addLayer(NeuralNetwork* nn, Layer* layer);
void readParameters(NeuralNetwork* nn, const char* filename, VectorPointers* matrices, VectorPointers* vectors, size_t qntyLayers);
void writeParameters(NeuralNetwork* nn, const char* filename);
void printParameters(NeuralNetwork* nn);
void updateParameters(NeuralNetwork* nn, double learningRate);
void calcActivations(NeuralNetwork* nn);
String calcCost(NeuralNetwork* nn, Vector* desiredOutput, unsigned char writeResult);
void writeIterationResult(NeuralNetwork* nn, const char* filename, const String* result);
void printIterationResult(NeuralNetwork* nn);

double dC_da(NeuralNetwork* nn, size_t neuron, size_t n_layer);
double dC_dw(NeuralNetwork* nn, double derivativeCostOverAct, size_t j, size_t k, size_t n_layer);
double dC_db(NeuralNetwork* nn, double derivativeCostOverAct, size_t j, size_t n_layer);

NeuralNetwork* NeuralNetwork_new(const char* name, size_t inputsNumber, size_t outputsNumber, Vector* hiddenNeuronsNumber, VectorPointers* hiddenActFn, ActivationFunction* outputActFn, unsigned char extract_from_file)
{
	NeuralNetwork* newNN = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
	ASSERT(newNN, MEMORY_NOT_ASSIGNED_EXCEPTION);
	ASSERT(name, N_NETWORK_MUST_HAVE_NAME);

	{
		size_t sizeOfName = strlen(name) + 1;
		newNN->name = (char*)malloc(sizeOfName);
		ASSERT(newNN->name, MEMORY_NOT_ASSIGNED_EXCEPTION);
		memcpy(newNN->name, name, sizeOfName);
	}

	newNN->hiddenLayers = VectorPointers_new(Layer_delete);
	newNN->input = Vector_new(inputsNumber);
	newNN->output = Vector_new(outputsNumber);
	newNN->averageCost = 10.0;
	newNN->qntyTests = 0;
	newNN->successfulTests = 0;
	newNN->totalCost = 0.0;

	Layer* inputLayer = Layer_new_input(inputsNumber);
	addLayer(newNN, inputLayer);
	
	newNN->parameters_management = DataManagement_new("data/");
	
	String fileNameString = String_create_container(10);
	String_emplace(&fileNameString, 2, name, " - Initial Parameters.txt");
	const char* fileName = fileNameString.Buffer;

	VectorPointers* weightsMatrices = VectorPointers_new_init_size(Matrix_delete, hiddenNeuronsNumber->size + 1);
	VectorPointers* biasesVectors = VectorPointers_new_init_size(Vector_delete, hiddenNeuronsNumber->size + 1);

	// Si se decide extraer los parámetros del archivo "name - Initial Parameters.txt"
	long position = 0;
	if (extract_from_file)
		readParameters(newNN, fileName, weightsMatrices, biasesVectors, hiddenNeuronsNumber->size + 1);

	size_t i;
	for (i = 0; i < hiddenNeuronsNumber->size; i++)
	{
		size_t inputs = newNN->lastLayer->output->size;
		
		ActivationFunction* actFn = ActivationFunction_new_copy((ActivationFunction*)VectorPointers_get(hiddenActFn, i));
		size_t neuronsNumber = (size_t)Vector_get(hiddenNeuronsNumber, i);
		Layer* newLayer;

		// Si se inicializan de manera aleatoria los parámetros, luego se los escribirán en el archivo "name - Initial Parameters.txt"
		if (!extract_from_file)
		{
			newLayer = Layer_new_1(HIDDEN_LAYER, neuronsNumber, actFn, inputs);
			addLayer(newNN, newLayer);
		}

		// Si se extrajeron los parámetros del archivo "name - Initial Parameters.txt", se los escriben en las matrices y los vectores de las capas ocultas
		else
		{
			Matrix* weights = (Matrix*)VectorPointers_get(weightsMatrices, i);
			Vector* biases = (Vector*)VectorPointers_get(biasesVectors, i);
			newLayer = Layer_new_4(HIDDEN_LAYER, neuronsNumber, actFn, inputs, weights, biases);
			addLayer(newNN, newLayer);
		}
		VectorPointers_add(newNN->hiddenLayers, newNN->lastLayer);
	}

	size_t inputs = newNN->lastLayer->output->size;

	Layer* newLayer;

	// Si se inicializan de manera aleatoria los parámetros, luego se los escribirán en el archivo "name - Initial Parameters.txt"
	if (!extract_from_file)
	{
		newLayer = Layer_new_1(OUTPUT_LAYER, outputsNumber, outputActFn, inputs);
		addLayer(newNN, newLayer);
	}

	// Si se extrajeron los parámetros del archivo "name - Initial Parameters.txt", se los escriben en las matrices y los vectores de la capa de salida
	else
	{
		Matrix* weights = (Matrix*)VectorPointers_get(weightsMatrices, i);
		Vector* biases = (Vector*)VectorPointers_get(biasesVectors, i);

		newLayer = Layer_new_4(OUTPUT_LAYER, outputsNumber, outputActFn, inputs, weights, biases);
		addLayer(newNN, newLayer);

		// Se controla si las dimensiones de las matrices y los vectores son correctas
		for (size_t l = 1; l < newNN->hiddenLayers->size + 2; l++)
		{
			Layer* layer = getLayer(newNN, l);
			size_t layerNeurons = l < newNN->hiddenLayers->size + 1 ? (size_t)Vector_get(hiddenNeuronsNumber, l - 1) : outputsNumber;
			ASSERT(layer->weights->rows == layerNeurons, MATRIX_INVALID_DIMS_EXCEPTION);
			ASSERT(layer->biases->size == layerNeurons, ARRAY_INVALID_DIMS_EXCEPTION);
		}
	}

	// Se escriben en el archivo "name - Initial Parameters.txt" los parámetros (los pesos y sesgos) de las capas de la red neuronal
	if (!extract_from_file)
		writeParameters(newNN, fileName);

	newNN->algorithm = Backpropagation_new(outputsNumber);

	VectorPointers_delete(weightsMatrices);
	VectorPointers_delete(biasesVectors);

	String_delete_stacked(&fileNameString);

	newNN->stringIteration = String_int_to_string(0);
	newNN->stringCost = String_double_to_string(1.0);

	puts("Neural network created!\n");
	
	String* stringNameOfNN = String_new_container(25);
	String_emplace(stringNameOfNN, 2,
		"Neural network name: ",
		name
	);

	GUI_AddWordNewLine(stringNameOfNN);
	GUI_AddWord(String_new_emplace("Iteration: "));
	GUI_AddWordNewLine(newNN->stringIteration);
	GUI_AddWord(String_new_emplace("Cost of the neural network: "));
	GUI_AddWordNewLine(newNN->stringCost);

	derivativesCostOverActivation = unordered_map_new_store_values(sizeof(Matrix_Position));

	return newNN;
}

void NeuralNetwork_delete(NeuralNetwork* nn)
{
	free(nn->name);
	nn->name = NULL;

	DataManagement_delete(nn->parameters_management);
	Backpropagation_delete(nn->algorithm);
	Layer_delete(nn->inputLayer);
	Vector_delete(nn->input);
	VectorPointers_delete(nn->hiddenLayers);
	Layer_delete(nn->outputLayer);
	Vector_delete(nn->output);
	//String_delete(nn->stringIteration);
	//String_delete(nn->stringCost);
	unordered_map_delete(derivativesCostOverActivation);

	free(nn);
}

void NeuralNetwork_single_input_learn(NeuralNetwork* nn, Vector* input, Vector* desiredOutput, double learningRate, unsigned char writeResult)
{
	Vector_set_another_vector(nn->input, input);
	calcActivations(nn);

	String train_result = calcCost(nn, desiredOutput, writeResult);

	updateParameters(nn, learningRate);

	/* De acá en adelante solo se imprimen los resultados de la iteración */

	printIterationResult(nn);

	if (writeResult)
	{
		String outputFile = String_create_container(10);
		String_emplace(&outputFile, 2, nn->name, " - Learning");
		writeIterationResult(nn, outputFile.Buffer, &train_result);
		
		String_delete_stacked(&outputFile);

		outputFile = String_create_container(10);
		String_emplace(&outputFile, 2, nn->name, " - Current Parameters.txt");
		writeParameters(nn, outputFile.Buffer);

		String_delete_stacked(&outputFile);
	}

	String_delete_stacked(&train_result);
}

Layer* getLayer(NeuralNetwork* nn, size_t index)
{
	if (index == 0)
		return nn->inputLayer;
	else if (index == nn->hiddenLayers->size + 1)
		return nn->outputLayer;
	else
		return VectorPointers_get(nn->hiddenLayers, index - 1);
}

void addLayer(NeuralNetwork* nn, Layer* layer)
{
	if (layer->layerType == INPUT_LAYER)
	{
		//layer->layerNumber = 0; Ya está hecho esto en la creación de la capa de entrada
		nn->inputLayer = layer;
		nn->lastLayer = layer;
	}
	else
	{
		short layerNumber = (short)(nn->hiddenLayers->size + 1);
		layer->layerNumber = layerNumber;

		// A la última capa de la red neuronal se le asigna ahora la capa nueva
		nn->lastLayer = layer;

		if (layer->layerType == OUTPUT_LAYER)
			nn->outputLayer = layer;
	}
}

void readParameters(NeuralNetwork* nn, const char* filename, VectorPointers* matrices, VectorPointers* vectors, size_t qntyLayers)
{
	DataManagement* de = nn->parameters_management;

	long position = 0;
	for (size_t i = 0; i < qntyLayers; i++)
	{
		position = DataManagement_readMatVec(de, filename, position);

		Matrix* mat = Matrix_new_copy(DataManagement_getMat(de));
		Vector* vec = Vector_new_copy(DataManagement_getVec(de));

		VectorPointers_add(matrices, mat);
		VectorPointers_add(vectors, vec);
	}
}

void writeParameters(NeuralNetwork* nn, const char* filename)
{
	DataManagement* de = nn->parameters_management;
	unsigned char firstIteration = 1;
	for (size_t l = 1; l < nn->hiddenLayers->size + 2; l++)
	{
		Layer* layer = getLayer(nn, l);
		DataManagement_set_data(de, layer->weights, layer->biases);

		if (firstIteration)
		{
			firstIteration = 0;
			DataManagement_writeMatVec(de, filename, OVERWRITE);
		}
		else
			DataManagement_writeMatVec(de, filename, APPEND);
	}

	DataManagement_release_data(de);
}

void printParameters(NeuralNetwork* nn)
{
	printf("Epoch: %d\n", nn->algorithm->iteration);
	printf("Cost: %lf\n\n", nn->averageCost);
	for (size_t i = 0; i < nn->hiddenLayers->size + 2; i++)
	{
		Layer* l = getLayer(nn, i);
		Matrix_show(l->weights);
		printf("\n");
		Vector_show(l->biases);
		printf("\n\t-------------- O --------------\n\n");
	}
}

void updateParameters(NeuralNetwork* nn, double learningRate)
{
	// Recorro todas las capas, de la última a la primera, para ajustar los parámetros según el gradiente, y al gradiente mismo
	for (size_t l = nn->hiddenLayers->size + 1; l != 0; l--)
	{
		Layer* layer = getLayer(nn, l);

		Vector gradientCostOverAct = Vector_create_zero(layer->neuronsInLayer);

		// Para la capa actual, asigno a un vector todas las derivadas del costo respecto a la activacón de cada neurona de la capa
		for (size_t i = 0; i < gradientCostOverAct.size; i++)
			Vector_set(&gradientCostOverAct, i, dC_da(nn, i, l));

		// Obtengo todas las derivadas del costo respecto a los pesos que conectan la capa anterior con la actual, y además los valores acumulados de estas derivadas
		Matrix* weightsGrad = layer->weightsGradient;
		Matrix* sumWeightsGrad = layer->sumWeightsGradient;

		// Obtengo todas las derivadas del costo respecto a los sesgos de cada neurona de la capa actual, y además los valores acumulados de estas derivadas
		Vector* biasesGrad = layer->biasesGradient;
		Vector* sumBiasesGrad = layer->sumBiasesGradient;

		// Itero sobre todas las neuronas de la capa actual para el cálculo
		for (size_t j = 0; j < weightsGrad->rows; j++)
		{
			// Recojo la derivada del costo respecto a la activación de la j-ésima neurona de la capa actual
			double DerivativeCostOverActivation = Vector_get(&gradientCostOverAct, j);

			// Obtengo el inverso de la cantidad de iteraciones que va realizando la red neuronal para el cálculo del promedio
			double calculateAverage = 1.0 / nn->algorithm->iteration;

			// Calculo la derivada del costo respecto al sesgo de la neurona actual
			double DerivativeCostOverBias = dC_db(nn, DerivativeCostOverActivation, j, l);

			// Sumo esta derivada al acumulador de derivada de sesgo para la neurona j
			double addedBiasDerivative = Vector_sum_element(sumBiasesGrad, j, DerivativeCostOverBias);

			// Asigno este acumulador, promediado por la cantidad total de iteraciones, al lugar correspondiente de las derivadas del costo respecto a los sesgos
			Vector_set(biasesGrad, j, calculateAverage * addedBiasDerivative);

			// Recorro los valores de los pesos que conectan a la neurona j con las neuronas de la capa anterior para calcular la derivada total del costo respecto a cada peso
			for (size_t k = 0; k < weightsGrad->columns; k++)
			{
				MAT_T DerivativeCostOverWeight = dC_dw(nn, DerivativeCostOverActivation, j, k, l);
				Matrix_Position pos = M_Pos(j, k);

				MAT_T addedWeightDerivative = Matrix_sum_element(sumWeightsGrad, pos, DerivativeCostOverWeight);
				Matrix_set(weightsGrad, pos, calculateAverage * addedWeightDerivative);
			}
		}

		Vector_delete_stacked(&gradientCostOverAct);

		Matrix* weightsGradWithLearningRate = Matrix_product_with_scalar(layer->weightsGradient, -learningRate);
		Matrix_sum_void(layer->weights, weightsGradWithLearningRate);

		Vector* biasesGradWithLearningRate = Vector_product_with_scalar(layer->biasesGradient, -learningRate);
		Vector_sum_void(layer->biases, biasesGradWithLearningRate);

		Matrix_delete(weightsGradWithLearningRate);
		Vector_delete(biasesGradWithLearningRate);
	}
#ifdef USE_MAP
	unordered_map_clear(derivativesCostOverActivation);
#endif
}

double dC_da(NeuralNetwork* nn, size_t neuron, size_t n_layer)
{
#ifdef USE_MAP
	Matrix_Position key = M_Pos(neuron, n_layer);
	double* gotValue = unordered_map_lookup(derivativesCostOverActivation, &key);
	if (gotValue != NULL)
		return *gotValue;
#endif
	//printf("Times dC_da() got executed: %d\n", ++dC_da_EnteredTimes);
	Layer* layer = getLayer(nn, n_layer);
	VEC_T actualOutput = Vector_get(layer->output, neuron);

	// Solo se usará esta variable cuando la capa a la que se hace referencia es la de salida
	VEC_T desiredOutput = 0.0;
	if (layer->layerType == OUTPUT_LAYER)
		desiredOutput = Vector_get(nn->algorithm->desiredOutput, neuron);

	double diffBetweenOutputAndDesiredOutput = actualOutput - desiredOutput;
	double applyAverage = 1.0 / layer->neuronsInLayer;

	if (layer->layerType == OUTPUT_LAYER)
	{
		double result = applyAverage * 2 * diffBetweenOutputAndDesiredOutput;
#ifdef USE_MAP
		unordered_map_insert(derivativesCostOverActivation, &key, &result);
#endif

		// La derivada del costo respecto a la activación de la última capa no es más que la derivada del error cuadrático medio. La diferencia entre la activación de la neurona de la última capa y la salida deseada en esta neurona es la que se calcula en el error cuadrático medio.
		return result;
	}
	else
	{
		Layer* next = getLayer(nn, n_layer + 1);
		size_t neuronsNextLayer = next->neuronsInLayer;
		double sum = 0.0;
		for (size_t j_index = 0; j_index < neuronsNextLayer; j_index++)
		{
			double outputActivation = Vector_get(next->outputBeforeActivation, j_index);
			double weight = Matrix_get(next->weights, M_Pos(j_index, neuron));
			sum += dC_da(nn, j_index, n_layer + 1) *
				deriv(next->actFunction, outputActivation) *
				weight
				;
		}
		return sum;
	}
}

double dC_dw(NeuralNetwork* nn, double derivativeCostOverAct, size_t j, size_t k, size_t n_layer)
{
	Layer* layer = getLayer(nn, n_layer);
	Layer* prevLayer = getLayer(nn, n_layer - 1);
	double outputActivation = Vector_get(layer->outputBeforeActivation, j);
	double outputPreviousLayer = Vector_get(prevLayer->output, k);

	return
		derivativeCostOverAct *
		deriv(layer->actFunction, outputActivation) *
		outputPreviousLayer
		;
}

double dC_db(NeuralNetwork* nn, double derivativeCostOverAct, size_t j, size_t n_layer)
{
	Layer* layer = getLayer(nn, n_layer);

	double outputActivation = Vector_get(layer->outputBeforeActivation, j);

	return
		derivativeCostOverAct *
		deriv(layer->actFunction, outputActivation)
		;
}

void calcActivations(NeuralNetwork* nn)
{
	// La capa de entrada calcula su salida según la función de activación que tiene asignada
	Vector_set_another_vector(nn->inputLayer->input, nn->input);
	Layer_outputCalculation(nn->inputLayer);

	Layer* lastHiddenLayer = nn->inputLayer;
	// Cada una de las capas ocultas calcula su salida según la salida de la capa anterior
	for (size_t i = 0; i < nn->hiddenLayers->size; i++)
	{
		Layer* hl = VectorPointers_get(nn->hiddenLayers, i);
		Layer* prevLayer = i == 0 ? nn->inputLayer : VectorPointers_get(nn->hiddenLayers, i - 1);
		Vector_set_another_vector(hl->input, prevLayer->output);
		Layer_outputCalculation(hl);
		
		if (i == nn->hiddenLayers->size - 1)
			lastHiddenLayer = VectorPointers_get(nn->hiddenLayers, i);
	}

	// Calculo la salida de la red
	Vector_set_another_vector(nn->outputLayer->input, lastHiddenLayer->output);
	Layer_outputCalculation(nn->outputLayer);
	Vector_set_another_vector(nn->output, nn->outputLayer->output);
}

String calcCost(NeuralNetwork* nn, Vector* desiredOutput, unsigned char writeResult)
{
	ASSERT(nn->output, MEMORY_POINTING_TO_NOTHING);
	ASSERT(desiredOutput->size == nn->output->size, N_NETWORK_DESIRED_OUTPUT_WRONG);

	nn->totalCost = updateCost(nn->algorithm, nn->output, desiredOutput, nn->totalCost);
	nn->averageCost = (1.0 / nn->algorithm->iteration) * nn->totalCost;

	String iterationResult = String_create_container(16);
	if (writeResult)
	{
		String inputVec = String_create_container(10);
		String_add(&inputVec, "[");
		for (size_t i = 0; i < nn->input->size; i++)
		{
			String inputElemStr = String_double_to_string_stacked(Vector_get(nn->input, i));
			String_add(&inputVec, inputElemStr.Buffer);
		
			if (i != nn->input->size - 1)
				String_add(&inputVec, ", ");
			else
				String_add(&inputVec, "]");

			String_delete_stacked(&inputElemStr);
		}

		String outputVecStr = String_create_container(10);
		String_add(&outputVecStr, "[");
		for (size_t i = 0; i < nn->output->size; i++)
		{
			String outputElemStr = String_double_to_string_stacked(Vector_get(nn->output, i));
			String_add(&outputVecStr, outputElemStr.Buffer);

			if (i != nn->output->size - 1)
				String_add(&outputVecStr, ", ");
			else
				String_add(&outputVecStr, "]");

			String_delete_stacked(&outputElemStr);
		}

		String desiredOutputStr = String_create_container(10);
		String_add(&desiredOutputStr, "[");
		for (size_t i = 0; i < desiredOutput->size; i++)
		{
			String desiredOutElemStr = String_double_to_string_stacked(Vector_get(desiredOutput, i));
			String_add(&desiredOutputStr, desiredOutElemStr.Buffer);

			if (i != nn->output->size - 1)
				String_add(&desiredOutputStr, ", ");
			else
				String_add(&desiredOutputStr, "]");

			String_delete_stacked(&desiredOutElemStr);
		}

		double individualCost = 0.0;
		for (size_t i = 0; i < nn->output->size; i++)
		{
			double outputElem = Vector_get(nn->output, i);
			double desiredOutputElem = Vector_get(desiredOutput, i);
			individualCost += pow(outputElem - desiredOutputElem, 2);
		}

		Vector* outputVec = nn->output;
		size_t neuronsInOutput = outputVec->size;
		individualCost /= neuronsInOutput;
	
		String individualCostStr = String_double_to_string_stacked(individualCost);

		String iteration = String_int_to_string_stacked(nn->algorithm->iteration);
		String averageCost = String_double_to_string_stacked(nn->averageCost);

		String_emplace(&iterationResult, 14,
			"Epoch: ",
			iteration.Buffer,
			"\nInput: ",
			inputVec.Buffer,
			"\nDesired output: ",
			desiredOutputStr.Buffer,
			"\nOutput: ",
			outputVecStr.Buffer,
			"\nCost: ",
			averageCost.Buffer,
			"\nIndividual cost: ",
			individualCostStr.Buffer,
			"\n",
			SEPARATOR
		);

		String_delete_stacked(&inputVec);
		String_delete_stacked(&outputVecStr);
		String_delete_stacked(&desiredOutputStr);
		String_delete_stacked(&individualCostStr);
		String_delete_stacked(&iteration);
		String_delete_stacked(&averageCost);
	}

	return iterationResult;
}

void writeIterationResult(NeuralNetwork* nn, const char* filename, const String* result)
{
	ASSERT(result, MEMORY_NOT_ASSIGNED_EXCEPTION);
	ASSERT(_mkdir("logs") != ENOENT, FOLDER_WRONG_PATH);

	String full_path = String_create_container(10);
	String_emplace(&full_path, 3, "logs/", filename, ".txt");

	FILE* fd;
	unsigned char overflowed = 0;
	if (counterWritingOverflow++ > 10000000)
	{
		counterWritingOverflow = 0;
		overflowed = 1;
	}

	const char* mode = firstTimeWriting == 1 || overflowed ? "w" : "a";
	if (firstTimeWriting)
		firstTimeWriting = 0;
	fopen_s(&fd, full_path.Buffer, mode);
	ASSERT(fd, FILE_WRONG_PATH);

	fprintf(fd, "%s", result->Buffer);

	String_delete_stacked(&full_path);

	fclose(fd);
}

void printIterationResult(NeuralNetwork* nn)
{
	String_set_int_to_string(nn->stringIteration, nn->algorithm->iteration);
	String_set_double_to_string(nn->stringCost, nn->averageCost);
}
