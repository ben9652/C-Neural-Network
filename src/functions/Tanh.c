#include "Tanh.h"
#include <math.h>

double calc_Tanh(Tanh* self, double x)
{
    return tanh(self->a * x);
}

double deriv_Tanh(Tanh* self, double x)
{
    return 1.0 / pow(cosh(self->a * x), 2.0);
}
