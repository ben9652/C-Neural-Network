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

typedef union {
	Hardlim h;
	Hardlims hs;
	Linear l;
	Tanh t;
	Sigmoid s;
} Functions;

typedef struct act_f {
	enum func_to_use func_used;
	Functions* function;
} ActivationFunction;

ActivationFunction* ActivationFunction_new(enum func_to_use function, double param, double rightShift);
ActivationFunction* ActivationFunction_new_copy(ActivationFunction* other);
void ActivationFunction_delete(ActivationFunction* self);

double calc(ActivationFunction* self, double x);
double deriv(ActivationFunction* self, double x);

#endif