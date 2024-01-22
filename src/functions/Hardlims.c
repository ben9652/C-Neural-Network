#include "Hardlims.h"

double calc_Hardlims(Hardlims* self, double x)
{
	return self->a * x >= 0 ? 1.0 : -1.0;
}

double deriv_Hardlims(Hardlims* self, double x)
{
	return 0.0;
}
