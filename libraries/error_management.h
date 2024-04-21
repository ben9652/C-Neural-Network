#ifndef __ERROR_MANAGEMENT_
#define __ERROR_MANAGEMENT_

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#define MEMORY_EXCEPTIONS				0x00000000
#define FUNCTION_EXCEPTIONS				0x01000000
#define MATRIX_EXCEPTIONS				0x02000000
#define ARRAY_EXCEPTIONS				0x03000000
#define FILE_EXCEPTIONS					0x04000000
#define PARSE_EXCEPTIONS				0x05000000

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

// Tipos de excepciones por Array
#define ARRAY_NULL_PASSED_EXCEPTION		ARRAY_EXCEPTIONS | 0x00
#define ARRAY_OUT_OF_BOUNDS_EXCEPTION	ARRAY_EXCEPTIONS | 0x01

// Tipos de excepciones por DataExtraction
#define FILE_NOT_EXISTING_EXCEPTION		FILE_EXCEPTIONS | 0x00

// Tipos de excepciones para conversiones de datos
#define PARSE_NOT_PARSED_EXCEPTION	PARSE_EXCEPTIONS | 0x00
#define PARSE_STRS_EXCEPTION		PARSE_EXCEPTIONS | 0x01

void ErrorLog(int code);

#define ASSERT(x, code)\
	if(!(x))\
	ErrorLog(code);\
	if(!(x))\
	__debugbreak();

#endif