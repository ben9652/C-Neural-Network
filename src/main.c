#include "DetectMemoryLeaks.h"
#include "Matrix.h"
#include "Vector.h"
#include "Vector_stress_tests.h"
#include "VectorPointers.h"
#include "NeuralNetwork.h"
#include "time_measurement.h"
#include "error_management.h"
#include "Random.h"
#include "StringVec.h"
#include <stdio.h>
#include <math.h>
#include "ConsolePositions.h"
//#include "Image.h"

#define WRITE_ITERATION 10000
#define TEST_NEURAL_NETWORK

const char* path = "data/";
const char* filename = "inputData.txt";

float getMatrixAdditionTime(Matrix* m1, Matrix* m2);
float getMatrixMultiplicationTime(Matrix* m1, Matrix* m2);
float getMatrixVectorProductTime(Matrix* mat, Vector* vec);
void miscellaneousTests();
float neuralNetworkTest();

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(294);

    float taken_time = neuralNetworkTest();

#ifndef TEST_NEURAL_NETWORK
    printf("Taken time: %f milliseconds\n", taken_time);
    fgetc(stdin);
#endif

    return 0;
}

float getMatrixAdditionTime(Matrix* m1, Matrix* m2)
{
    init_time_measurement();

    Matrix_sum(m1, m2);

    finalize_time_measurement();

    return get_elapsed_time(MEASURE_MILLISECONDS);
}

float getMatrixMultiplicationTime(Matrix* m1, Matrix* m2)
{
    init_time_measurement();

    Matrix_mul(m1, m2);

    finalize_time_measurement();

    return get_elapsed_time(MEASURE_MILLISECONDS);
}

float getMatrixVectorProductTime(Matrix* mat, Vector* vec)
{
    init_time_measurement();

    Vector_mat_vec_product(mat, vec);

    finalize_time_measurement();

    return get_elapsed_time(MEASURE_MILLISECONDS);
}

void miscellaneousTests()
{
    Vector* vec = Vector_new_emplace(5, 2, 16, 7, 6, 1);

    Vector v = Vector_create_emplace(5, 2, 16, 7, 0, 2);

    Vector_delete(vec);
    Vector_delete_stacked(&v);
}

float neuralNetworkTest()
{
#ifdef TEST_NEURAL_NETWORK
    double learningRate;
    double cost;
    printf("Enter the learning rate you want for the neural network training: ");
    int error = scanf_s("%lf", &learningRate);

    printf("Enter the cost you want for the network: ");
    error = scanf_s("%lf", &cost);
#else
    double learningRate = 0.5;
    double cost = 0.01;
#endif
    
    const char* neuralNetwork_Name = "XOR";
    size_t neuralNetwork_Inputs = 2;
    size_t neuralNetwork_Outputs = 2;
    size_t neuralNetwork_Hiddens = 1;
    Vector* neuralNetwork_HiddenNeurons = Vector_new_emplace(neuralNetwork_Hiddens, 3);
    VectorPointers* neuralNetwork_hiddenActFn = VectorPointers_new_init(ActivationFunction_delete, neuralNetwork_Hiddens, ActivationFunction_new(SIGMOID, 1.0, 0.0));
    ActivationFunction* neuralNetwork_OutputActFn = ActivationFunction_new(SIGMOID, 1.0, 0.0);
    unsigned char readDataFromFile = 0;

    NeuralNetwork* xorNeuralNetwork = NeuralNetwork_new(
        neuralNetwork_Name,
        neuralNetwork_Inputs,
        neuralNetwork_Outputs,
        neuralNetwork_HiddenNeurons,
        neuralNetwork_hiddenActFn,
        neuralNetwork_OutputActFn,
        readDataFromFile
    );

    Vector* input1 = Vector_new_emplace(2, 0, 0);
    Vector* input2 = Vector_new_emplace(2, 0, 1);
    Vector* input3 = Vector_new_emplace(2, 1, 0);
    Vector* input4 = Vector_new_emplace(2, 1, 1);

    VectorPointers* inputs = VectorPointers_new_init(Vector_delete, 4, input1, input2, input3, input4);

    Vector* desiredOutput1 = Vector_new_emplace(2, 1, 0);
    Vector* desiredOutput2 = Vector_new_emplace(2, 0, 1);
    Vector* desiredOutput3 = Vector_new_emplace(2, 0, 1);
    Vector* desiredOutput4 = Vector_new_emplace(2, 1, 0);

    VectorPointers* desiredOutputs = VectorPointers_new_init(Vector_delete, 4,
        desiredOutput1,
        desiredOutput2,
        desiredOutput3,
        desiredOutput4
    );

    init_time_measurement();
    
    size_t counter = 0;
    size_t i = 0;
    while (1)
    {
        size_t randIndex = (size_t)genRandomInt(0, 3);
        Vector* input = VectorPointers_get(inputs, randIndex);
        Vector* desiredOutput = VectorPointers_get(desiredOutputs, randIndex);

        if (i > 100 && xorNeuralNetwork->averageCost < cost)
        {
            NeuralNetwork_learn(xorNeuralNetwork, input, desiredOutput, learningRate, 1);
            break;
        }

        if (++counter == WRITE_ITERATION)
        {
            NeuralNetwork_learn(xorNeuralNetwork, input, desiredOutput, learningRate, 1);
            counter = 0;
        }
        else
            NeuralNetwork_learn(xorNeuralNetwork, input, desiredOutput, learningRate, 0);

        i++;
    }
    
    finalize_time_measurement();

    float taken_time = get_elapsed_time(MEASURE_MILLISECONDS);
#ifdef TEST_NEURAL_NETWORK
    float fSeconds = taken_time / 1000;

    float fMinutes = fSeconds / 60;
    float minutes_decimalPart = (fMinutes - (int)fMinutes);

    float fHours = fMinutes / 60;
    float hours_decimalPart = (fHours - (int)fHours);

    fSeconds = minutes_decimalPart * 60;
    fMinutes = hours_decimalPart * 60;

    int seconds = (int)roundf(fSeconds);
    int minutes = (int)roundf(fMinutes);
    int hours = (int)roundf(fHours);

    String* strToWrite = String_new_container(10);

    String* strIts = String_int_to_string(xorNeuralNetwork->algorithm->iteration);
    String* costOfNN = String_double_to_string(xorNeuralNetwork->averageCost);
    String* strSecs = String_int_to_string(seconds);
    String* strMins = String_int_to_string(minutes);
    String* strHours = String_int_to_string(hours);
    String_emplace(strToWrite, 13,
        "Iterations: ",
        strIts->Buffer,
        "\n",
        "Average cost of the neural network: ",
        costOfNN->Buffer,
        "\n\n",
        "Training time: ",
        strHours->Buffer,
        "h ",
        strMins->Buffer,
        "m ",
        strSecs->Buffer,
        "s\n"
    );

    String_delete(strIts);
    String_delete(costOfNN);
    String_delete(strSecs);
    String_delete(strMins);
    String_delete(strHours);
    
    FILE* fd;
    fopen_s(&fd, "Training results.txt", "w");
    ASSERT(fd, FILE_DOESNT_EXISTS_EXCEPTION);
    fprintf(fd, strToWrite->Buffer);
    fclose(fd);

    String_delete(strToWrite);
#endif
    NeuralNetwork_delete(xorNeuralNetwork);

    Vector_delete(neuralNetwork_HiddenNeurons);

    VectorPointers_delete(neuralNetwork_hiddenActFn);
    VectorPointers_delete(inputs);
    VectorPointers_delete(desiredOutputs);

    return taken_time;
}
