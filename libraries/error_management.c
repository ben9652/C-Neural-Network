#include "error_management.h"

void ErrorLog(int code)
{
	switch (code)
	{
	case MEMORY_NOT_ASSIGNED_EXCEPTION:
		puts("Error assigning memory");
		return;
	case MEMORY_POINTING_TO_NOTHING:
		puts("Pointer's not pointing to a memory space");
		return;
	case FUNCTION_ASSIGNING_EXCEPTION:
		puts("Wrong function assigned to ActivationFunction");
		return;
	case MATRIX_NOT_CREATED_EXCEPTION:
		puts("Failed creating matrix");
		return;
	case MATRIX_NULL_PASSED_EXCEPTION:
		puts("Null matrix passed to the function");
		return;
	case MATRIX_OUT_OF_BOUNDS_EXCEPTION:
		puts("Out of range matrix index");
		return;
	case MATRIX_INVALID_MULTIPLICATION:
		puts("Invalid matrices dimensions for multiplication");
		return;
	case ARRAY_NULL_PASSED_EXCEPTION:
		puts("Null vector passed to the function");
		return;
	case ARRAY_OUT_OF_BOUNDS_EXCEPTION:
		puts("Out of range vector index");
		return;
	case FILE_NOT_EXISTING_EXCEPTION:
		puts("Error trying to open the file");
		return;
	case PARSE_NOT_PARSED_EXCEPTION:
		puts("Couldn't convert");
		return;
	case PARSE_STRS_EXCEPTION:
		puts("Couldn't convert some characters");
		return;
	}
}