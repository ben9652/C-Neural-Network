#ifndef DATA_MANAGEMENT
#define DATA_MANAGEMENT
#include <stdio.h>
#include <stdlib.h>
#include "Matrix.h"
#include "Vector.h"

typedef struct data_management DataManagement;

typedef enum
{
	OVERWRITE,
	APPEND
} Write_Mode;

DataManagement* DataManagement_new(const char* main_path);

void DataManagement_delete(DataManagement* de);

void DataManagement_set_data(DataManagement* de, Matrix* mat, Vector* vec);
void DataManagement_release_data(DataManagement* de);

long DataManagement_readMatVec(DataManagement* de, const char* filename, long position);
void DataManagement_writeMatVec(DataManagement* de, const char* filename, Write_Mode write_mode);

Matrix* DataManagement_getMat(DataManagement* de);
Vector* DataManagement_getVec(DataManagement* de);

#endif