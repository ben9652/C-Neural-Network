#ifndef __ERROR_MANAGEMENT_
#define __ERROR_MANAGEMENT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

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
#define U_MAP_EXCEPTIONS				0x09000000

// Tipos de excepciones de memoria
#define MEMORY_NOT_ASSIGNED_EXCEPTION	MEMORY_EXCEPTIONS | 0x00
#define MEMORY_POINTING_TO_NOTHING		MEMORY_EXCEPTIONS | 0x01

// Tipos de excepciones por ActivationFunction
#define FUNCTION_ASSIGNING_EXCEPTION	FUNCTION_EXCEPTIONS | 0x00

// Tipos de excepciones por Matrix
#define MATRIX_NOT_CREATED_EXCEPTION	MATRIX_EXCEPTIONS | 0x00
#define MATRIX_NULL_PASSED_EXCEPTION	MATRIX_EXCEPTIONS | 0x01
#define MATRIX_OUT_OF_BOUNDS_EXCEPTION	MATRIX_EXCEPTIONS | 0x02
#define MATRIX_INVALID_DIMS_EXCEPTION	MATRIX_EXCEPTIONS | 0x03

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

// Tipos de excepciones para mapas desordenados
#define U_MAP_KEY_NULL					U_MAP_EXCEPTIONS | 0x00
#define U_MAP_OBJ_NULL					U_MAP_EXCEPTIONS | 0x01

void ErrorLog(int code);

static size_t ptNumCharsConv;
static wchar_t wStr[200];
static LPWSTR ptr;

#ifndef _DIST
#define PRINT_ERR(str) puts(str);
#else
#define PRINT_ERR(str)\
mbstowcs_s(&ptNumCharsConv, wStr, 200, str, strlen(str) + 1);\
ptr = wStr;\
MessageBox(NULL, ptr, u"Error", MB_ICONERROR | MB_OK);
#endif // !_DIST

#ifdef _DEBUG
#define BREAK_EXECUTION() __debugbreak();
#elif NDEBUG
#define BREAK_EXECUTION()\
Beep(1000, 1000);\
printf("\n\nError source: %s (line %d)\n", __FILENAME__, __LINE__);\
puts("The program will be exit...");\
fgetc(stdin);\
exit(1);
#else
#define BREAK_EXECUTION()\
exit(EXIT_FAILURE);
#endif

#define ASSERT(x, code)\
	if(!(x))\
	{\
		ErrorLog(code);\
		BREAK_EXECUTION();\
	}

#define ASSERT_STR(x, str)\
	if(!(x))\
	{\
		PRINT_ERR(str);\
		BREAK_EXECUTION();\
	}

#endif	// _DEBUG