#ifndef __HARDLIMS_
#define __HARDLIMS_

typedef struct hs_f
{
	double a;
} Hardlims;

double calc_Hardlims(Hardlims* self, double x);
double deriv_Hardlims(Hardlims* self, double x);

#endif