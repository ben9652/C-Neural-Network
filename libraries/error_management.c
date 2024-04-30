#include "error_management.h"
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>

void ErrorLog(int code, const char* file, int line)
{
	switch (code)
	{
	case MEMORY_NOT_ASSIGNED_EXCEPTION:
		puts("Error assigning memory");
		break;
	case MEMORY_POINTING_TO_NOTHING:
		puts("Pointer's not pointing to a memory space");
		break;
	case FUNCTION_ASSIGNING_EXCEPTION:
		puts("Wrong function assigned to ActivationFunction");
		break;
	case MATRIX_NOT_CREATED_EXCEPTION:
		puts("Failed creating matrix");
		break;
	case MATRIX_NULL_PASSED_EXCEPTION:
		puts("Null matrix passed to the function");
		break;
	case MATRIX_OUT_OF_BOUNDS_EXCEPTION:
		puts("Out of range matrix index");
		break;
	case MATRIX_INVALID_MULTIPLICATION:
		puts("Invalid matrices dimensions for multiplication");
		break;
	case MATRIX_INVALID_ADDITION:
		puts("Invalid matrices dimensions for addition");
		break;
	case ARRAY_NULL_PASSED_EXCEPTION:
		puts("Null vector passed to the function");
		break;
	case ARRAY_OUT_OF_BOUNDS_EXCEPTION:
		puts("Out of range vector index");
		break;
	case ARRAY_INVALID_DIMS_EXCEPTION:
		puts("Dimensions of the vectors doesn't match for the operation");
		break;
	case FILE_WRONG_PATH:
		puts("Error trying to open the file");
		break;
	case FILE_DOESNT_EXISTS_EXCEPTION:
		puts("The file you're trying to open doesn't exists");
		break;
	case FOLDER_WRONG_PATH:
		puts("Choosed path doesn't exists");
		break;
	case PARSE_NOT_PARSED_EXCEPTION:
		puts("Couldn't convert");
		break;
	case PARSE_STRS_EXCEPTION:
		puts("Couldn't convert some characters");
		break;
	case N_LAYER_CANT_BE_INPUT:
		puts("The layer of neurons can't be an input");
		break;
	case N_LAYER_WITHOUT_INPUT:
		puts("The layer of neurons doesn't have an input");
		break;
	case N_NETWORK_MUST_HAVE_NAME:
		puts("The neural network must have a name");
		break;
	case N_NETWORK_DESIRED_OUTPUT_WRONG:
		puts("The desired output's dimension doesn't match the network's output");
		break;
	case IMAGE_SDL_ERROR_INIT:
		printf("Error initializing SDL: %s\n", SDL_GetError());
		break;
	case IMAGE_SDL_IMAGE_ERROR_INIT:
		printf("Error initializing SDL_image: %s\n", IMG_GetError());
		SDL_Quit();
		break;
	case IMAGE_FAILED_LOAD_IMAGE:
		printf("Error loading image: %s\n", SDL_GetError());
		SDL_Quit();
		break;
	case IMAGE_FAILED_CONVERTING_IMAGE:
		printf("Error converting image: %s\n", SDL_GetError());
		break;
	}

#ifndef _DEBUG
	{
		Beep(1000, 1000);
		printf("\n\nError source: %s (line %d)\n", file, line);
		puts("The program will be exit...");
	}
#endif

}
