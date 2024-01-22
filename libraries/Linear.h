#ifndef __LINEAL_
#define __LINEAL_

typedef struct lin_f
{
	double a;
} Linear;

double calc_Linear(Linear* self, double x);
double deriv_Linear(Linear* self, double x);

#endif