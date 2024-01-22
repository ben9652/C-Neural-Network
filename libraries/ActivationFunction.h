#ifndef __FUNCION_ACTIVACION_
#define __FUNCION_ACTIVACION_
#include "Hardlim.h"
#include "Hardlims.h"
#include "Linear.h"
#include "Sigmoid.h"
#include "Tanh.h"

enum func_to_use {
	HARDLIM,
	HARDLIMS,
	LINEAR,
	SIGMOID,
	TANH
};

typedef struct act_f {
	char func_used[2];
} ActivationFunction;

ActivationFunction* new_ActivationFunction1(enum func_to_use function, double param);
ActivationFunction* new_ActivationFunction2(enum func_to_use function, double param, double rightShift);
void delete_ActivationFunction(ActivationFunction* self);

double calc(ActivationFunction* self, double x);
double deriv(ActivationFunction* self, double x);

#endif