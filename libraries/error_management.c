#include "error_management.h"

jmp_buf savebuf;
int exception_value = 0;

void ErrorLog()
{
	switch (exception_value)
	{
	case NOT_ASSIGNED_EXCEPTION:
		printf("Error assigning memory");
		break;
	case FUNCTION_ASSIGNING_EXCEPTION:
		printf("Wrong function assigned to ActivationFunction");
		break;
	}

	printf("\n");
}