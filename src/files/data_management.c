#include "data_management.h"
#include "error_management.h"
#include <string.h>
#include <direct.h>

struct data_management
{
	char* path;
	Matrix* mat;
	Vector* vec;
};

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

	if(main_path != NULL && strcmp(main_path, "./") != 0)
		ASSERT(_mkdir(main_path) != ENOENT, FOLDER_WRONG_PATH);

	de->mat = NULL;
	de->vec = NULL;

	return de;
}

void DataManagement_delete(DataManagement* de)
{
	free(de->path);
	free(de);
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

	DataManagement_release_data(de);

	de->mat = Matrix_new_copy(mat);
	de->vec = Vector_new_copy(vec);
}

void DataManagement_release_data(DataManagement* de)
{
	ASSERT(de, MEMORY_POINTING_TO_NOTHING);

	Matrix_delete(de->mat);
	de->mat = NULL;
	Vector_delete(de->vec);
	de->vec = NULL;
}

long DataManagement_readMatVec(DataManagement* de, const char* filename, long position)
{
	ASSERT(de, MEMORY_POINTING_TO_NOTHING);
	ASSERT(filename, MEMORY_POINTING_TO_NOTHING);
	const char* path_to_file = get_full_path(de, filename);
	FILE* fd;
	ASSERT(de, MEMORY_NOT_ASSIGNED_EXCEPTION);
	fopen_s(&fd, path_to_file, "r");
	ASSERT(fd, FILE_DOESNT_EXISTS_EXCEPTION);

	free((char*)path_to_file);

	if(position != 0)
		fseek(fd, position, SEEK_SET);

	char line[2000];
	char* token;
	size_t row = 0;

	size_t rows = countRows(fd);
	Matrix* mat = NULL;
	Vector* vec = NULL;

	Vector* first_row = Vector_new_container(10);

	// Flag para determinar cuándo se inicia el vector
	unsigned char vectorStart = 0;

	while (fgets(line, sizeof(line), fd) != NULL)
	{
		if (strcmp(line, "\n") == 0)
		{
			// Si ya se guardó el vector, se rompe el bucle indicando que ya se recogió la matriz y el vector
			if (vectorStart)
				break;

			// Si no se recogió el vector, estamos en una línea con un caracter '\n', y encima la matriz es nula, evidentemente es porque estamos en el proceso de obtener otra matriz y otro vector por consecuencia de habernos posicionado más adelante en el archivo mediante el parámetro `position`
			else if (mat == NULL)
				continue;

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
				if(strcmp(token, "\n") == 0)
					break;
				number = convertToDouble(token);

				// Me fijo, con un vector dinámico, cuál es la cantidad de columnas. Y cuando ya las obtuve, agrego elementos en la matriz ya creada.
				if (row == 0)
					Vector_add(first_row, number);
				else
					Matrix_set(mat, M_Pos(row, col), number);

				token = strtok_s(NULL, ",", &next_tok);
				col++;
			}

			// Al obtener la cantidad total de columnas, ya puedo crear una matriz fija
			if (row == 0)
			{
				mat = Matrix_new_null(rows, col);

				VEC_T* data = (VEC_T*)first_row->Buffer;
				Matrix_set_row(mat, row, data, first_row->size);

				Vector_delete(first_row);
				first_row = Vector_new_container(10);
			}
			row++;
		}

		// Si hemos llegado al vector
		else
		{
			token = strtok_s(line, ",", &next_tok);
			while (token != NULL)
			{
				if (strcmp(token, "\n") == 0)
					break;
				number = convertToDouble(token);
				Vector_add(first_row, number);
				token = strtok_s(NULL, ",", &next_tok);
			}
			
			vec = Vector_new_array(first_row->Buffer, first_row->size);
			Vector_delete(first_row);
		}
	}

	DataManagement_set_data(de, mat, vec);

	Matrix_delete(mat);
	Vector_delete(vec);

	long currentPosition = ftell(fd);

	fclose(fd);

	return currentPosition;
}

void DataManagement_writeMatVec(DataManagement* de, const char* filename, Write_Mode write_mode)
{
	ASSERT(de, MEMORY_POINTING_TO_NOTHING);
	ASSERT(filename, MEMORY_POINTING_TO_NOTHING);
	const char* path_to_file = get_full_path(de, filename);
	FILE* fd;
	ASSERT(de, MEMORY_NOT_ASSIGNED_EXCEPTION);
	fopen_s(&fd, path_to_file, write_mode == APPEND ? "a" : "w");
	ASSERT(fd, FILE_WRONG_PATH);

	free((char*)path_to_file);

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
			fprintf(fd, "%.17lf,", matrix_element);
		}
		fprintf(fd, "\n");
	}

	// Separador entre matriz y vector
	fprintf(fd, "\n");

	// Escribir el vector
	for (size_t i = 0; i < vec->size; i++)
		fprintf(fd, "%.17lf,", Vector_get(vec, i));
	
	fprintf(fd, "\n\n");

	fclose(fd);
}

Matrix* DataManagement_getMat(DataManagement* de)
{
	return de->mat;
}

Vector* DataManagement_getVec(DataManagement* de)
{
	return de->vec;
}
