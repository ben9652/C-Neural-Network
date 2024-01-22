#include "Sigmoid.h"
#include <math.h>

double calc_Sigmoid(Sigmoid* self, double x)
{
    return 1.0 / (1.0 + exp(- self->a * x + self->b));
}

double deriv_Sigmoid(Sigmoid* self, double x)
{
    double sigmoid = calc_Sigmoid(self, x);
    return self->a * sigmoid * (1 - sigmoid);
}
