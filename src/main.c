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
#include "GUI.h"
#include <pthread.h>
#include "SharedVariables.h"
#include "ThreadsManagement.h"
#include <Windows.h>
//#include "Image.h"

#define WRITE_ITERATION 10000
#define NEURAL_NETWORK_TRAINING_INPUT 2
#define TEST_NEURAL_NETWORK

struct Window* wnd_hnd;
unsigned char neural_network_learned = 0;

void* neuralNetworkTest(Vector* const input);
int Main();

sem_t NN_Thread_Management;

#ifdef _DIST
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
    {
        return Main();
    }
#else
    int main()
    {
        return Main();
    }
#endif // _DIST

int Main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(134);

    wnd_hnd = GUI_InitWindow(740, 480);
    if (wnd_hnd == NULL)
        return 1;

    // Configuro un semáforo para parar el proceso de testeo de la red neuronal hasta que se ingresen los parámetros de entrada del test
    sem_init(&NN_Thread_Management, 0, 0);

    VectorPointers* labelInputs = NULL;

#ifdef TEST_NEURAL_NETWORK
    labelInputs = VectorPointers_new_init(String_delete, NEURAL_NETWORK_TRAINING_INPUT,
        String_new_emplace("Learning rate: "),
        String_new_emplace("Minimum neural network\'s cost to reach: ")
    );
#endif

    Vector* const inputVector = Vector_new_container(NEURAL_NETWORK_TRAINING_INPUT);

    pthread_t nn_thread;
    pthread_create(&nn_thread, NULL, neuralNetworkTest, (Vector* const)inputVector);

    GUIloop(labelInputs, inputVector);
    pthread_cancel(nn_thread);

    if(!neural_network_learned)
        VectorPointers_delete(labelInputs);
    Vector_delete(inputVector);

    return 0;
}

void* neuralNetworkTest(Vector* const input)
{
    double learningRate;
    double cost;
#ifdef TEST_NEURAL_NETWORK
    sem_wait(&NN_Thread_Management);
    learningRate = Vector_get(input, 0);
    cost = Vector_get(input, 1);
#else
    learningRate = 0.5;
    cost = 0.01;
#endif
    
    const char* neuralNetwork_Name = "XOR";
    size_t neuralNetwork_Inputs = 2;
    size_t neuralNetwork_Outputs = 1;
    size_t neuralNetwork_Hiddens = 1;
    Vector* neuralNetwork_HiddenNeurons = Vector_new_emplace(neuralNetwork_Hiddens, 3);
    VectorPointers* neuralNetwork_hiddenActFn = VectorPointers_new_init(ActivationFunction_delete, neuralNetwork_Hiddens, ActivationFunction_new(SIGMOID, 1.0, 0.0));
    ActivationFunction* neuralNetwork_OutputActFn = ActivationFunction_new(SIGMOID, 1.0, 0.0);
    unsigned char readDataFromFile = 1;

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

    Vector* desiredOutput1 = Vector_new_emplace(1, 0);
    Vector* desiredOutput2 = Vector_new_emplace(1, 1);
    Vector* desiredOutput3 = Vector_new_emplace(1, 1);
    Vector* desiredOutput4 = Vector_new_emplace(1, 0);

    VectorPointers* desiredOutputs = VectorPointers_new_init(Vector_delete, 4,
        desiredOutput1,
        desiredOutput2,
        desiredOutput3,
        desiredOutput4
    );

#ifdef TEST_NEURAL_NETWORK
    init_time_measurement();
    
    size_t counter = 0;
    size_t i = 0;
    while (1)
    {
        size_t randIndex = i % 4;
        Vector* input = VectorPointers_get(inputs, randIndex);
        Vector* desiredOutput = VectorPointers_get(desiredOutputs, randIndex);

        if (i > 100 && xorNeuralNetwork->averageCost < cost)
        {
            NeuralNetwork_single_input_learn(xorNeuralNetwork, input, desiredOutput, learningRate, 1);
            break;
        }

        if (++counter == WRITE_ITERATION)
        {
            NeuralNetwork_single_input_learn(xorNeuralNetwork, input, desiredOutput, learningRate, 1);
            counter = 0;
        }
        else
            NeuralNetwork_single_input_learn(xorNeuralNetwork, input, desiredOutput, learningRate, 0);

        i++;
    }
    
    finalize_time_measurement();

    float taken_time = get_elapsed_time(MEASURE_MILLISECONDS);
    float fSeconds = taken_time / 1000;

    float fMilliseconds = (fSeconds - (int)fSeconds) * 1000;

    float fMinutes = fSeconds / 60;
    float minutes_decimalPart = (fMinutes - (int)fMinutes);

    float fHours = fMinutes / 60;
    float hours_decimalPart = (fHours - (int)fHours);

    fSeconds = minutes_decimalPart * 60;
    fMinutes = hours_decimalPart * 60;

    int milliseconds = (int)fMilliseconds;
    int seconds = (int)roundf(fSeconds);
    int minutes = (int)roundf(fMinutes);
    int hours = (int)roundf(fHours);

    String* strToWrite = String_new_container(10);

    String* strIts = String_int_to_string(xorNeuralNetwork->algorithm->iteration);
    String* costOfNN = String_double_to_string(xorNeuralNetwork->averageCost);
    String* strMs = String_int_to_string(milliseconds);
    String* strSecs = String_int_to_string(seconds);
    String* strMins = String_int_to_string(minutes);
    String* strHours = String_int_to_string(hours);
    String_emplace(strToWrite, 15,
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
        "s ",
        strMs->Buffer,
        "ms\n"
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
#else
    NeuralNetwork_learn(xorNeuralNetwork, input1, desiredOutput1, learningRate, 1);
#endif
    NeuralNetwork_delete(xorNeuralNetwork);

    Vector_delete(neuralNetwork_HiddenNeurons);

    VectorPointers_delete(neuralNetwork_hiddenActFn);
    VectorPointers_delete(inputs);
    VectorPointers_delete(desiredOutputs);

#ifdef TEST_NEURAL_NETWORK
    neural_network_learned = 1;
    GUI_AddNewLine();
    GUI_AddWordNewLine(String_new_emplace("The neural network learned until the specified cost!"));
    GUI_AddWordNewLine(String_new_emplace("ENTER to exit..."));
#endif

    return NULL;
}