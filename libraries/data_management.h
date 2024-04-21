#ifndef DATA_MANAGEMENT
#define DATA_MANAGEMENT
#include <stdio.h>
#include <stdlib.h>
#include "Matrix.h"
#include "Vector.h"

typedef struct
{
	char* path;
	Matrix* mat;
	Vector* vec;
} DataManagement;

DataManagement* DataManagement_new(const char* main_path);

void DataManagement_set_data(DataManagement* de, Matrix* mat, Vector* vec);
void DataManagement_release_data(DataManagement* de);

void DataManagement_readMatVec(DataManagement* de, const char* filename);
void DataManagement_writeMatVec(DataManagement* de, const char* filename);

#endif