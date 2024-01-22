#ifndef __ERROR_MANAGEMENT_
#define __ERROR_MANAGEMENT_

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

// Tipos de excepciones de memoria
#define NOT_ASSIGNED_EXCEPTION 0x00000000

// Tipos de excepciones por ActivationFunction
#define FUNCTION_ASSIGNING_EXCEPTION 0x10000000

void ErrorLog();

extern jmp_buf savebuf;
extern int exception_value;

#define ASSERT(x) if(!(x)) __debugbreak();

#define TRY if ((exception_value = setjmp(savebuf)) == 0)
#define CATCH(NUM) else if (exception_value == NUM)
#define CATCHALL else
#define THROW(NUM) longjmp(savebuf, NUM);

#endif