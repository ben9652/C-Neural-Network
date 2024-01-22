#include "Linear.h"
#include <math.h>

double calc_Linear(Linear* self, double x)
{
    return fmax(0.0, self->a * x);
}

double deriv_Linear(Linear* self, double x)
{
    return self->a * x >= 0 ? 1.0 : 0.0;
}
