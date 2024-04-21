#include "ActivationFunction.h"
#include <stdlib.h>
#include <string.h>
#include "error_management.h"

unsigned char private_members;

/* Funciones para obtener los atributos privados */

enum func_to_use getFuncUsed(ActivationFunction* af);

/* Miembros privados */

enum func_to_use function;
typedef union {
	Hardlim h;
	Hardlims hs;
	Linear l;
	Tanh t;
} Functions;

// Constructor
ActivationFunction* new_ActivationFunction1(enum func_to_use function, double param)
{
	ASSERT(function != SIGMOID, FUNCTION_ASSIGNING_EXCEPTION);

	size_t size_of_struct = sizeof(ActivationFunction);

	size_t size_of_privMember1 = sizeof(enum func_to_use);
	size_t size_of_privMember2 = sizeof(Functions);

	size_t size_of_private =  size_of_privMember1 + size_of_privMember2;

	ActivationFunction* af = malloc(size_of_struct + size_of_private);

	if (af)
	{
		enum func_to_use* p_Func = (enum func_to_use*)(af + 1);
		*p_Func = function;

		Functions* p_Actual_Func = (Functions*)(af + 3);

		switch (function)
		{
		case HARDLIM:
			strcpy_s(af->func_used, 2, "h");
			p_Actual_Func->h.a = param;
			break;
		case HARDLIMS:
			strcpy_s(af->func_used, 2, "H");
			p_Actual_Func->hs.a = param;
			break;
		case LINEAR:
			strcpy_s(af->func_used, 2, "l");
			p_Actual_Func->l.a = param;
			break;
		case TANH:
			strcpy_s(af->func_used, 2, "t");
			p_Actual_Func->t.a = param;
			break;
		}
	}

	return af;
}

ActivationFunction* new_ActivationFunction2(enum func_to_use function, double param, double rightShift)
{
	ASSERT(function == SIGMOID, FUNCTION_ASSIGNING_EXCEPTION);

	size_t size_of_struct = sizeof(ActivationFunction);

	size_t size_of_privMember1 = sizeof(enum func_to_use);
	size_t size_of_privMember2 = sizeof(Sigmoid);

	size_t size_of_private = size_of_privMember1 + size_of_privMember2;

	ActivationFunction* af = malloc(size_of_struct + size_of_private);

	if (af)
	{
		enum func_to_use* p_Func = (enum func_to_use*)(af + 1);
		*p_Func = function;

		Sigmoid* p_Actual_Func = (Sigmoid*)(af + 3);

		strcpy_s(af->func_used, 2, "s");
		p_Actual_Func->a = param;
		p_Actual_Func->b = rightShift;
	}

	return af;
}

void delete_ActivationFunction(ActivationFunction* self)
{
	free(self);
}

double calc(ActivationFunction* self, double x)
{
	enum func_to_use function = getFuncUsed(self);

	void* func_pointer = self + 3;

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
		return 0.0;
	}
}

double deriv(ActivationFunction* self, double x)
{
	return 0.0;
}

enum func_to_use getFuncUsed(ActivationFunction* af)
{
	enum func_to_use* function = (enum func_to_use*)(af + 1);
	return *function;
}
