#include "error_management.h"
#include <SDL.h>
#include <SDL_image.h>

void ErrorLog(int code)
{
	switch (code)
	{
	case MEMORY_NOT_ASSIGNED_EXCEPTION:
		PRINT_ERR("Error assigning memory");
		break;
	case MEMORY_POINTING_TO_NOTHING:
		PRINT_ERR("Pointer's not pointing to a memory space");
		break;
	case FUNCTION_ASSIGNING_EXCEPTION:
		PRINT_ERR("Wrong function assigned to ActivationFunction");
		break;
	case MATRIX_NOT_CREATED_EXCEPTION:
		PRINT_ERR("Failed creating matrix");
		break;
	case MATRIX_NULL_PASSED_EXCEPTION:
		PRINT_ERR("Null matrix passed to the function");
		break;
	case MATRIX_OUT_OF_BOUNDS_EXCEPTION:
		PRINT_ERR("Out of range matrix index");
		break;
	case MATRIX_INVALID_MULTIPLICATION:
		PRINT_ERR("Invalid matrices dimensions for multiplication");
		break;
	case MATRIX_INVALID_ADDITION:
		PRINT_ERR("Invalid matrices dimensions for addition");
		break;
	case ARRAY_NULL_PASSED_EXCEPTION:
		PRINT_ERR("Null vector passed to the function");
		break;
	case ARRAY_OUT_OF_BOUNDS_EXCEPTION:
		PRINT_ERR("Out of range vector index");
		break;
	case ARRAY_INVALID_DIMS_EXCEPTION:
		PRINT_ERR("Dimensions of the vectors doesn't match for the operation");
		break;
	case FILE_WRONG_PATH:
		PRINT_ERR("Error trying to open the file");
		break;
	case FILE_DOESNT_EXISTS_EXCEPTION:
		PRINT_ERR("The file you're trying to open doesn't exists");
		break;
	case FOLDER_WRONG_PATH:
		PRINT_ERR("Choosed path doesn't exists");
		break;
	case PARSE_NOT_PARSED_EXCEPTION:
		PRINT_ERR("Couldn't convert");
		break;
	case PARSE_STRS_EXCEPTION:
		PRINT_ERR("Couldn't convert some characters");
		break;
	case N_LAYER_CANT_BE_INPUT:
		PRINT_ERR("The layer of neurons can't be an input");
		break;
	case N_LAYER_WITHOUT_INPUT:
		PRINT_ERR("The layer of neurons doesn't have an input");
		break;
	case N_NETWORK_MUST_HAVE_NAME:
		PRINT_ERR("The neural network must have a name");
		break;
	case N_NETWORK_DESIRED_OUTPUT_WRONG:
		PRINT_ERR("The desired output's dimension doesn't match the network's output");
		break;
	}
}
