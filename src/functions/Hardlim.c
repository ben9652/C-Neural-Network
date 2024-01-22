#include "Hardlim.h"

double calc_Hardlim(Hardlim* self, double x)
{
	return self->a * x >= 0 ? 1.0 : 0.0;
}

double deriv_Hardlim(Hardlim* self, double x)
{
	return 0.0;
}