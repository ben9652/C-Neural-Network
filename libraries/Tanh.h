#ifndef __TANH_
#define __TANH_

typedef struct tanh_f
{
	double a;
} Tanh;

double calc_Tanh(Tanh* self, double x);
double deriv_Tanh(Tanh* self, double x);

#endif