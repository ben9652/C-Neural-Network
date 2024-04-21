#ifndef __GENERIC_MANAGEMENT
#define __GENERIC_MANAGEMENT

#define UNSIGNED_CHAR_T							0x01
#define CHAR_T									0x02

#define UNSIGNED_SHORT_T						0x03
#define SHORT_T									0x04

#define UNSIGNED_INT_T							0x05
#define INT_T									0x06

#define UNSIGNED_LONG_T							0x07
#define LONG_T									0x08

#define UNSIGNED_LONG_LONG_T					0x09
#define LONG_LONG_T								0x0A

#define FLOAT_T									0x0B

#define DOUBLE_T								0x0C
#define LONG_DOUBLE_T							0x0D

#define POINTER_TO_UNSIGNED_CHAR_T				0x0E
#define POINTER_TO_CHAR_T						0x0F

#define POINTER_TO_UNSIGNED_SHORT_T				0x10
#define POINTER_TO_SHORT_T						0x11

#define POINTER_TO_UNSIGNED_INT_T				0x12
#define POINTER_TO_INT_T						0x13

#define POINTER_TO_UNSIGNED_LONG_T				0x14
#define POINTER_TO_LONG_T						0x15

#define POINTER_TO_UNSIGNED_LONG_LONG_T			0x16
#define POINTER_TO_LONG_LONG_T					0x17

#define POINTER_TO_FLOAT_T						0x18

#define POINTER_TO_DOUBLE_T						0x19
#define POINTER_TO_LONG_DOUBLE_T				0x1A

#define INVALID_TYPE							0xFF

#define typeof(x) _Generic(x,										\
	/* Datatypes*/													\
	unsigned char: UNSIGNED_CHAR_T,									\
	char: CHAR_T,													\
																	\
	unsigned short: UNSIGNED_SHORT_T,								\
	short: SHORT_T,													\
																	\
	unsigned int: UNSIGNED_INT_T,									\
	int: INT_T,														\
																	\
	unsigned long: UNSIGNED_LONG_T,									\
	long: LONG_T,													\
																	\
	unsigned long long: UNSIGNED_LONG_LONG_T,						\
	long long: LONG_LONG_T,											\
																	\
	float: FLOAT_T,													\
																	\
	double: DOUBLE_T,												\
	long double: LONG_DOUBLE_T,										\
																	\
	/* Pointers to types*/											\
	unsigned char*: POINTER_TO_UNSIGNED_CHAR_T,						\
	char*: POINTER_TO_CHAR_T,										\
																	\
	unsigned short*: POINTER_TO_UNSIGNED_SHORT_T,					\
	short*: POINTER_TO_SHORT_T,										\
																	\
	unsigned int*: POINTER_TO_UNSIGNED_INT_T,						\
	int*: POINTER_TO_INT_T,											\
																	\
	unsigned long*: POINTER_TO_UNSIGNED_LONG_T,						\
	long*: POINTER_TO_LONG_T,										\
																	\
	unsigned long long*: POINTER_TO_UNSIGNED_LONG_LONG_T,			\
	long long*: POINTER_TO_LONG_LONG_T,								\
																	\
	float*: POINTER_TO_FLOAT_T,										\
																	\
	double*: POINTER_TO_DOUBLE_T,									\
	long double*: POINTER_TO_LONG_DOUBLE_T,							\
																	\
	default: INVALID_TYPE											\
)

#define typename(x) _Generic((x),									\
	unsigned char: "unsigned char",									\
	char: "char",													\
																	\
	unsigned short: "unsigned short",								\
	short: "short",													\
																	\
	unsigned int: "unsigned int",									\
	int: "int",														\
																	\
	unsigned long: "unsigned long",									\
	long: "long",													\
																	\
	unsigned long long: "unsigned long long",						\
	long long: "long long",											\
																	\
	float: "float",													\
																	\
	double: "double",												\
	long double: "long double",										\
																	\
	/* Pointers to types*/											\
	unsigned char*: "unsigned char*",								\
	char*: "char*",													\
																	\
	unsigned short*: "unsigned short*",								\
	short*: "short*",												\
																	\
	unsigned int*: "unsigned int*",									\
	int*: "int*",													\
																	\
	unsigned long*: "unsigned long*",								\
	long*: "long*",													\
																	\
	unsigned long long*: "unsigned long long*",						\
	long long*: "long long*",										\
																	\
	float*: "float*",												\
																	\
	double*: "double*",												\
	long double*: "long double*"									\
)

#endif