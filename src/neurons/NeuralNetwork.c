#include "NeuralNetwork.h"
#include "error_management.h"
#include "StringVec.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <direct.h>
#include "DetectMemoryLeaks.h"

#define SEPARATOR "\n\t\t-------------------- O --------------------\n\n"
#define TIME_TO_PRINT 50000

static unsigned char firstTimeWriting = 1;
static size_t counterWritingOverflow = 0;

void addLayer(NeuralNetwork* nn, Layer* layer);
void readParameters(NeuralNetwork* nn, const char* filename, VectorPointers* matrices, VectorPointers* vectors, size_t qntyLayers);
void writeParameters(NeuralNetwork* nn, const char* filename);
void printParameters(NeuralNetwork* nn);
void calcActivations(NeuralNetwork* nn);
String calcCost(NeuralNetwork* nn, Vector* desiredOutput, unsigned char writeResult);
void writeIterationResult(NeuralNetwork* nn, const char* filename, const String* result);
void printIterationResult(NeuralNetwork* nn);

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
	}

	// Se escriben en el archivo "name - Initial Parameters.txt" los parámetros (los pesos y sesgos) de las capas de la red neuronal
	if (!extract_from_file)
		writeParameters(newNN, fileName);

	newNN->algorithm = Backpropagation_new(outputsNumber);

	VectorPointers_delete(weightsMatrices);
	VectorPointers_delete(biasesVectors);

	String_delete_stacked(&fileNameString);

	CursorPosition* cursorEpoch = CursorPosition_new();
	newNN->cursorEpoch = cursorEpoch;
	
	CursorPosition* cursorCost = CursorPosition_new();
	newNN->cursorCost = cursorCost;

	puts("Neural network created!\n");
	printf("Neural network name: %s\n", name);

	printf("Iteration: ");
	UpdateCursorPosition(cursorEpoch);
	printf("\nAverage cost: ");
	UpdateCursorPosition(cursorCost);
	printf("\n\n");
	
	newNN->counterForPrinting = 0;

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
	CursorPosition_delete(nn->cursorEpoch);
	CursorPosition_delete(nn->cursorCost);

	free(nn);
}

void NeuralNetwork_learn(NeuralNetwork* nn, Vector* input, Vector* desiredOutput, double learningRate, unsigned char writeResult)
{
	Vector_set_another_vector(nn->input, input);
	calcActivations(nn);

	String train_result = calcCost(nn, desiredOutput, writeResult);

	for (Layer* l = nn->outputLayer; l->layerType != INPUT_LAYER; l = l->prevLayer)
		Layer_learn(l, nn->algorithm, learningRate);

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

	printIterationResult(nn);

	String_delete_stacked(&train_result);
}

void NeuralNetwork_test(NeuralNetwork* nn, Vector* input, Vector* desiredOutput, unsigned char writeResult)
{
	Vector_set_another_vector(nn->input, input);
	calcActivations(nn);

	String test_result = String_create_container(10);

	double outputElem;
	double desiredOutputElem;

	double cost = 0.0;
	for (size_t i = 0; i < nn->output->size; i++)
	{
		outputElem = Vector_get(nn->output, i);
		desiredOutputElem = Vector_get(desiredOutput, i);
		cost += pow(outputElem - desiredOutputElem, 2);
	}
	cost /= nn->output->size;

	nn->qntyTests++;
	if (cost < nn->tolerance)
		nn->successfulTests++;

	double successfultTests = (double)nn->successfulTests;
	double qntyTests = (double)nn->qntyTests;

	nn->performance = successfultTests / qntyTests;
	
	double individualCost = 0.0;
	for (size_t i = 0; i < nn->output->size; i++)
	{
		outputElem = Vector_get(nn->output, i);
		desiredOutputElem = Vector_get(desiredOutput, i);
		individualCost += pow(outputElem - desiredOutputElem, 2);
	}
	individualCost /= nn->output->size;

	String individualCostStr = String_double_to_string_stacked(individualCost);

	String desiredOutputVec = String_create_emplace("[");
	String desiredOutputElemStr;
	for (size_t i = 0; i < desiredOutput->size; i++)
	{
		desiredOutputElemStr = String_double_to_string_stacked(Vector_get(desiredOutput, i));
		String_add(&desiredOutputVec, desiredOutputElemStr.Buffer);
		if (i != nn->output->size - 1)
			String_add(&desiredOutputVec, ", ");
		else
			String_add(&desiredOutputVec, "]");
	}


	String performanceStr = String_create_emplace(String_double_to_string(nn->performance * 100)->Buffer);

	if (writeResult)
	{
		String_emplace(&test_result, 8,
			"Expected output: ",
			desiredOutputVec.Buffer,
			"\nIndividual cost: ",
			individualCostStr.Buffer,
			"\nPerformance: ",
			performanceStr.Buffer,
			" %\n",
			SEPARATOR
		);

		String outputFile = String_create_container(10);
		String_emplace(&outputFile, 2, nn->name, " - Tests");
		writeIterationResult(nn, "test", &test_result);
	}

	String_delete_stacked(&test_result);
	String_delete_stacked(&individualCostStr);
	String_delete_stacked(&desiredOutputVec);
	String_delete_stacked(&desiredOutputElemStr);
	String_delete_stacked(&performanceStr);
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

		// A esta capa nueva se le asigna como capa previa la última capa de la red neuronal
		layer->prevLayer = nn->lastLayer;

		// A la última capa de la red neuronal se le asigna como siguiente capa a la capa nueva
		nn->lastLayer->nextLayer = layer;

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
	for (Layer* l = nn->inputLayer->nextLayer; l != NULL; l = l->nextLayer)
	{
		DataManagement_set_data(de, l->weights, l->biases);

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
	for (Layer* l = nn->inputLayer->nextLayer; l != NULL; l = l->nextLayer)
	{
		Matrix_show(l->weights);
		printf("\n");
		Vector_show(l->biases);
		printf("\n\t-------------- O --------------\n\n");
	}
}

void calcActivations(NeuralNetwork* nn)
{
	Vector_set_another_vector(nn->inputLayer->input, nn->input);
	Layer_outputCalculation(nn->inputLayer);

	for (size_t i = 0; i < nn->hiddenLayers->size; i++)
	{
		Layer* hl = VectorPointers_get(nn->hiddenLayers, i);
		Vector_set_another_vector(hl->input, hl->prevLayer->output);
		Layer_outputCalculation(hl);
	}

	Vector_set_another_vector(nn->outputLayer->input, nn->outputLayer->prevLayer->output);
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
	if (nn->counterForPrinting == 0)
	{
		COORD coordsEpoch = GetCursorPosition(nn->cursorEpoch);
		COORD coordsCost = GetCursorPosition(nn->cursorCost);

		PositionCursor(nn->cursorEpoch, coordsEpoch);
		printf("%u", nn->algorithm->iteration);

		PositionCursor(nn->cursorCost, coordsCost);
		printf("%.17lf", nn->averageCost);

		nn->counterForPrinting = TIME_TO_PRINT - 1;
	}
	else
		nn->counterForPrinting--;
}
