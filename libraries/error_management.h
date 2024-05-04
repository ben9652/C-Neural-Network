#ifndef __ERROR_MANAGEMENT_
#define __ERROR_MANAGEMENT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define MEMORY_EXCEPTIONS				0x00000000
#define FUNCTION_EXCEPTIONS				0x01000000
#define MATRIX_EXCEPTIONS				0x02000000
#define ARRAY_EXCEPTIONS				0x03000000
#define FILE_EXCEPTIONS					0x04000000
#define FOLDER_EXCEPTIONS				0x05000000
#define PARSE_EXCEPTIONS				0x06000000
#define N_LAYER_EXCEPTIONS				0x07000000
#define N_NETWORK_EXCEPTIONS			0x08000000
#define IMAGE_EXCEPTIONS				0x09000000
#define GLFW_EXCEPTIONS					0x0A000000
#define IMPL_OPENGL3_EXCEPTIONS			0x0B000000

// Tipos de excepciones de memoria
#define MEMORY_NOT_ASSIGNED_EXCEPTION	MEMORY_EXCEPTIONS | 0x00
#define MEMORY_POINTING_TO_NOTHING		MEMORY_EXCEPTIONS | 0x01

// Tipos de excepciones por ActivationFunction
#define FUNCTION_ASSIGNING_EXCEPTION	FUNCTION_EXCEPTIONS | 0x00

// Tipos de excepciones por Matrix
#define MATRIX_NOT_CREATED_EXCEPTION	MATRIX_EXCEPTIONS | 0x00
#define MATRIX_NULL_PASSED_EXCEPTION	MATRIX_EXCEPTIONS | 0x01
#define MATRIX_OUT_OF_BOUNDS_EXCEPTION	MATRIX_EXCEPTIONS | 0x02
#define MATRIX_INVALID_MULTIPLICATION	MATRIX_EXCEPTIONS | 0x03
#define MATRIX_INVALID_ADDITION			MATRIX_EXCEPTIONS | 0x04

// Tipos de excepciones por Array
#define ARRAY_NULL_PASSED_EXCEPTION		ARRAY_EXCEPTIONS | 0x00
#define ARRAY_OUT_OF_BOUNDS_EXCEPTION	ARRAY_EXCEPTIONS | 0x01
#define ARRAY_INVALID_DIMS_EXCEPTION	ARRAY_EXCEPTIONS | 0x02

// Tipos de excepciones por manejos de archivos
#define FILE_WRONG_PATH				FILE_EXCEPTIONS | 0x00
#define FILE_DOESNT_EXISTS_EXCEPTION	FILE_EXCEPTIONS | 0x01

// Tipos de excepciones por manipulación de carpetas
#define FOLDER_WRONG_PATH				FOLDER_EXCEPTIONS | 0x00

// Tipos de excepciones para conversiones de datos
#define PARSE_NOT_PARSED_EXCEPTION		PARSE_EXCEPTIONS | 0x00
#define PARSE_STRS_EXCEPTION			PARSE_EXCEPTIONS | 0x01

// Tipos de excepciones para capas de neuronas
#define N_LAYER_CANT_BE_INPUT			N_LAYER_EXCEPTIONS | 0x00
#define N_LAYER_WITHOUT_INPUT			N_LAYER_EXCEPTIONS | 0x01

// Tipos de excepciones para redes neuronales
#define N_NETWORK_MUST_HAVE_NAME		N_NETWORK_EXCEPTIONS | 0x00
#define N_NETWORK_DESIRED_OUTPUT_WRONG	N_NETWORK_EXCEPTIONS | 0x01

// Tipos de excepciones para imágenes
#define IMAGE_SDL_ERROR_INIT			IMAGE_EXCEPTIONS | 0x00
#define IMAGE_SDL_IMAGE_ERROR_INIT		IMAGE_EXCEPTIONS | 0x01
#define IMAGE_FAILED_LOAD_IMAGE			IMAGE_EXCEPTIONS | 0x02
#define IMAGE_FAILED_CONVERTING_IMAGE	IMAGE_EXCEPTIONS | 0x03

// Tipos de excepciones para GLFW
#define GLFW_PLATFORM_ALREADY_INIT		GLFW_EXCEPTIONS | 0x00
#define GLFW_WND_PROC_NOT_NULL			GLFW_EXCEPTIONS | 0x01

// Tipos de excepciones para ImplOpenGL3
#define OPENGL3_NO_RENDERER_BACKEND		IMPL_OPENGL3_EXCEPTIONS | 0x00
#define OPENGL3_NOT_INIT				IMPL_OPENGL3_EXCEPTIONS | 0x01

void ErrorLog(int code, const char* file, int line);

#ifdef _DEBUG
#define BREAK_EXECUTION() __debugbreak();
#else
#define BREAK_EXECUTION()\
fgetc(stdin);\
exit(1);
#endif

#define ASSERT(x, code)\
	if(!(x))\
	{\
		ErrorLog(code, __FILENAME__, __LINE__);\
		BREAK_EXECUTION();\
	}
#endif