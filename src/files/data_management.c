#include "data_management.h"
#include "error_management.h"
#include <string.h>
#include "vc_vector/vc_vector.h"

#define MAX_LINE_LENGTH 2000

DataManagement* DataManagement_new(const char* main_path)
{
	size_t sizeOfPath = 0;
	if (main_path != NULL)
		sizeOfPath = strlen(main_path) + 1;

	DataManagement* de = (DataManagement*)malloc(sizeof(DataManagement));
	ASSERT(de, MEMORY_NOT_ASSIGNED_EXCEPTION);

	if (sizeOfPath)
		de->path = (char*)malloc(sizeOfPath);
	else
		de->path = (char*)malloc(3);

	ASSERT(de->path, MEMORY_NOT_ASSIGNED_EXCEPTION);

	if(main_path == NULL)
		strcpy_s(de->path, 3, "./");
	else
		strcpy_s(de->path, sizeOfPath, main_path);

	return de;
}

double convertToDouble(const char* str)
{
	char* ptr;
	double number;

	number = strtod(str, &ptr);

	ASSERT(ptr != str, PARSE_NOT_PARSED_EXCEPTION);
	ASSERT(*ptr == '\0', PARSE_STRS_EXCEPTION);

	return number;
}

size_t countRows(FILE* fd)
{
	size_t count = 0;

	char c;
	long int initialPosition = ftell(fd);

	while ((c = fgetc(fd)) != EOF)
	{
		if (c == '\n')
			count++;
		else if (c == '\r')
		{
			if (fgetc(fd) == '\n')
				count++;
		}

		if (c == '\n' && fgetc(fd) == '\n')
			break;
	}
	
	fseek(fd, initialPosition, SEEK_SET);

	return count;
}

char* get_full_path(DataManagement* de, const char* filename)
{
	size_t sizeOfPath = strlen(de->path);
	size_t sizeOfFilename = strlen(filename);
	size_t totalPathSize = sizeOfFilename + sizeOfPath;

	char* path_to_file = (char*)malloc(totalPathSize + 1);
	ASSERT(path_to_file, MEMORY_NOT_ASSIGNED_EXCEPTION);
	strcpy_s(path_to_file, sizeOfPath + 1, de->path);
	strcpy_s(path_to_file + sizeOfPath, sizeOfFilename + 1, filename);

	return path_to_file;
}

void DataManagement_set_data(DataManagement* de, Matrix* mat, Vector* vec)
{
	ASSERT(de, MEMORY_POINTING_TO_NOTHING);
	ASSERT(mat, MATRIX_NULL_PASSED_EXCEPTION);
	ASSERT(vec, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(mat->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(vec->Buffer, MEMORY_POINTING_TO_NOTHING);

	de->mat = Matrix_new_copy(mat);
	de->vec = Vector_new_copy(vec);
}

void DataManagement_release_data(DataManagement* de)
{
	Matrix_delete(de->mat);
	Vector_delete(de->vec);
}

void DataManagement_readMatVec(DataManagement* de, const char* filename)
{
	ASSERT(de, MEMORY_POINTING_TO_NOTHING);
	ASSERT(filename, MEMORY_POINTING_TO_NOTHING);
	const char* path_to_file = get_full_path(de, filename);
	FILE* fd;
	ASSERT(de, MEMORY_NOT_ASSIGNED_EXCEPTION);
	fopen_s(&fd, path_to_file, "r");
	ASSERT(fd, FILE_NOT_EXISTING_EXCEPTION);

	char line[2000];
	char* token;
	size_t row = 0;

	size_t rows = countRows(fd);
	Matrix* mat = NULL;
	Vector* vec = NULL;

	vc_vector* first_row = vc_vector_create(10, sizeof(VEC_T), NULL);

	// Flag para determinar cuándo se inicia el vector
	unsigned char vectorStart = 0;

	while (fgets(line, sizeof(line), fd) != NULL)
	{
		if (strcmp(line, "\n") == 0)
		{
			vectorStart = 1;
			continue;
		}

		VEC_T number;
		char* next_tok = NULL;
		// Si aún no hemos llegado al vector, leemos la matriz
		if (!vectorStart)
		{
			token = strtok_s(line, ",", &next_tok);
			size_t col = 0;
			while (token != NULL)
			{
				number = convertToDouble(token);

				// Me fijo, con un vector dinámico, cuál es la cantidad de columnas. Y cuando ya las obtuve, agrego elementos en la matriz ya creada.
				if (row == 0)
					vc_vector_push_back(first_row, &number);
				else
					Matrix_set(mat, M_Pos(row, col), number);

				token = strtok_s(NULL, ",\n", &next_tok);
				col++;
			}

			// Al obtener la cantidad total de columnas, ya puedo crear una matriz fija
			if (row == 0)
			{
				mat = Matrix_new_null(rows, col);

				VEC_T* data = (VEC_T*)vc_vector_data(first_row);
				Matrix_set_row(mat, row, data, vc_vector_count(first_row));

				vc_vector_release(first_row);
				first_row = vc_vector_create(10, sizeof(VEC_T), NULL);
			}
			row++;
		}

		// Si hemos llegado al vector
		else
		{
			token = strtok_s(line, ",", &next_tok);
			while (token != NULL)
			{
				number = convertToDouble(token);
				vc_vector_push_back(first_row, &number);
				token = strtok_s(NULL, ",", &next_tok);
			}
			
			vec = Vector_new_array((VEC_T*)vc_vector_data(first_row), vc_vector_count(first_row));
			vc_vector_release(first_row);
		}
	}

	ASSERT(mat, MEMORY_NOT_ASSIGNED_EXCEPTION);
	ASSERT(vec, MEMORY_NOT_ASSIGNED_EXCEPTION);
	de->mat = mat;
	de->vec = vec;

	fclose(fd);
}

void DataManagement_writeMatVec(DataManagement* de, const char* filename)
{
	ASSERT(de, MEMORY_POINTING_TO_NOTHING);
	ASSERT(filename, MEMORY_POINTING_TO_NOTHING);
	const char* path_to_file = get_full_path(de, filename);
	FILE* fd;
	ASSERT(de, MEMORY_NOT_ASSIGNED_EXCEPTION);
	fopen_s(&fd, path_to_file, "w");
	ASSERT(fd, FILE_NOT_EXISTING_EXCEPTION);

	ASSERT(de->mat, MATRIX_NULL_PASSED_EXCEPTION);
	ASSERT(de->vec, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(de->mat->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(de->vec->Buffer, MEMORY_POINTING_TO_NOTHING);

	Matrix* mat = de->mat;
	Vector* vec = de->vec;

	// Escribir la matriz
	for (size_t i = 0; i < mat->rows; i++)
	{
		for (size_t j = 0; j < mat->columns; j++)
		{
			MAT_T matrix_element = Matrix_get(mat, M_Pos(i, j));
			fprintf(fd, "%.17lf", matrix_element);
			if (j < mat->columns - 1)
				fprintf(fd, ",");
		}
		fprintf(fd, "\n");
	}

	// Separador entre matriz y vector
	fprintf(fd, "\n");

	// Escribir el vector
	for (size_t i = 0; i < vec->size; i++)
	{
		fprintf(fd, "%.17lf", Vector_get(vec, i));
		if (i < vec->size - 1)
			fprintf(fd, ",");
	}

	fclose(fd);
}
