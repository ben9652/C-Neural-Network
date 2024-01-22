#ifndef __SIGMOIDEA_
#define __SIGMOIDEA_

typedef struct sig_f
{
	double a;
	double b;
} Sigmoid;

double calc_Sigmoid(Sigmoid* self, double x);
double deriv_Sigmoid(Sigmoid* self, double x);

#endif