#ifndef __HARDLIM_
#define __HARDLIM_

typedef struct h_f
{
	double a;
} Hardlim;

double calc_Hardlim(Hardlim* self, double x);
double deriv_Hardlim(Hardlim* self, double x);

#endif