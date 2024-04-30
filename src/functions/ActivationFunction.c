#include "ActivationFunction.h"
#include <stdlib.h>
#include <string.h>
#include "error_management.h"

// Constructor
ActivationFunction* ActivationFunction_new(enum func_to_use function, double param, double rightShift)
{
	ActivationFunction* af = (ActivationFunction*)malloc(sizeof(ActivationFunction));
	ASSERT(af, MEMORY_NOT_ASSIGNED_EXCEPTION);
	af->func_used = function;

	Functions* func_to_use = (Functions*)malloc(sizeof(Functions));
	af->function = func_to_use;

	switch (function)
	{
	case HARDLIM:
		Hardlim h = { param };
		memmove(func_to_use, &h, sizeof(h));
		break;
	case HARDLIMS:
		Hardlims hs = { param };
		memmove(func_to_use, &hs, sizeof(hs));
		break;
	case LINEAR:
		Linear lr = { param };
		memmove(func_to_use, &lr, sizeof(lr));
		break;
	case TANH:
		Tanh tanh = { param };
		memmove(func_to_use, &tanh, sizeof(hs));
		break;
	case SIGMOID:
		Sigmoid s = { param, rightShift };
		memmove(func_to_use, &s, sizeof(s));
		break;
	}

	return af;
}

ActivationFunction* ActivationFunction_new_copy(ActivationFunction* other)
{
	ASSERT(other, MEMORY_POINTING_TO_NOTHING);
	ActivationFunction* newActFn;

	double param = 0.0;
	double right_shift = 0.0;
	switch (other->func_used)
	{
	case HARDLIM:
		
	case HARDLIMS:
		param = other->function->h.a;
		break;
	case LINEAR:
		param = other->function->l.a;
		break;
	case TANH:
		param = other->function->t.a;
		break;
	case SIGMOID:
		param = other->function->s.a;
		right_shift = other->function->s.b;
		break;
	}

	newActFn = ActivationFunction_new(other->func_used, param, right_shift);

	return newActFn;
}

void ActivationFunction_delete(ActivationFunction* self)
{
	ASSERT(self, MEMORY_POINTING_TO_NOTHING);
	ASSERT(self->function, MEMORY_POINTING_TO_NOTHING);
	
	free(self->function);
	free(self);
}

double calc(ActivationFunction* self, double x)
{
	enum func_to_use function = self->func_used;

	Functions* func_pointer = self->function;

	switch (function)
	{
	case HARDLIM:
		return calc_Hardlim((Hardlim*)func_pointer, x);
	case HARDLIMS:
		return calc_Hardlims((Hardlims*)func_pointer, x);
	case LINEAR:
		return calc_Linear((Linear*)func_pointer, x);
	case SIGMOID:
		return calc_Sigmoid((Sigmoid*)func_pointer, x);
	case TANH:
		return calc_Tanh((Tanh*)func_pointer, x);
	default:
		ASSERT(0, FUNCTION_ASSIGNING_EXCEPTION);
		return 0.0;
	}
}

double deriv(ActivationFunction* self, double x)
{
	enum func_to_use function = self->func_used;

	Functions* func_pointer = self->function;

	switch (function)
	{
	case HARDLIM:
		return deriv_Hardlim((Hardlim*)func_pointer, x);
	case HARDLIMS:
		return deriv_Hardlims((Hardlims*)func_pointer, x);
	case LINEAR:
		return deriv_Linear((Linear*)func_pointer, x);
	case SIGMOID:
		return deriv_Sigmoid((Sigmoid*)func_pointer, x);
	case TANH:
		return deriv_Tanh((Tanh*)func_pointer, x);
	default:
		ASSERT(0, FUNCTION_ASSIGNING_EXCEPTION);
		return 0.0;
	}
}
