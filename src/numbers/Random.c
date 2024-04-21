#include "Random.h"
#include <time.h>
#include <stdlib.h>

static unsigned char randomInitialized;

double genRandomDouble(double min, double max)
{
    // Inicializar la semilla para la función rand() utilizando el tiempo actual
    if (!randomInitialized)
    {
        srand((unsigned int)time(NULL));
        randomInitialized = 1;
    }

    double random_num = (double)rand() / RAND_MAX;

    double scaled_random = random_num * (max - min) + min;

    return scaled_random;
}

int genRandomInt(int min, int max)
{
    // Inicializar la semilla para la función rand() utilizando el tiempo actual
    if (!randomInitialized)
    {
        srand((unsigned int)time(NULL));
        randomInitialized = 1;
    }

    // Generar un número aleatorio en el rango [min, max]
    return min + rand() % (max - min + 1);
}