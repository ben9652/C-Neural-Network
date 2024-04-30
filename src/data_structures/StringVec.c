#include "StringVec.h"
#include "error_management.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "DetectMemoryLeaks.h"

String* const String_new(size_t size)
{
	String* res = (String*)malloc(sizeof(String));
	ASSERT(res, MEMORY_NOT_ASSIGNED_EXCEPTION);

	res->Buffer = (char*)malloc(size);
	ASSERT(res->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res->size = size;

	return res;
}

String String_create(size_t size)
{
	String res;

	res.Buffer = (char*)malloc(size);
	ASSERT(res.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res.size = size;

	return res;
}

String* const String_new_emplace(const char* string)
{
	ASSERT(string, MEMORY_POINTING_TO_NOTHING);
	String* newString = String_new(strlen(string) + 1);
	ASSERT(newString, MEMORY_NOT_ASSIGNED_EXCEPTION);

	memmove(newString->Buffer, string, strlen(string) + 1);

	return newString;
}

String String_create_emplace(const char* string)
{
	ASSERT(string, MEMORY_POINTING_TO_NOTHING);
	String newString = String_create(strlen(string) + 1);

	memmove(newString.Buffer, string, strlen(string) + 1);

	return newString;
}

String* const String_new_container(size_t size)
{
	size_t capacity = size + 16;
	String* res = (String*)malloc(sizeof(String));
	ASSERT(res, MEMORY_NOT_ASSIGNED_EXCEPTION);

	res->Buffer = (char*)malloc(capacity);
	ASSERT(res->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res->size = 0;
	res->capacity_in_bytes = capacity;

	return res;
}

String String_create_container(size_t size)
{
	size_t capacity = size + 16;
	String res;

	res.Buffer = (char*)malloc(capacity);
	ASSERT(res.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	res.size = 0;
	res.capacity_in_bytes = capacity;

	return res;
}

String* const String_new_copy(const String * newString)
{
	ASSERT(newString, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(newString->Buffer, MEMORY_POINTING_TO_NOTHING);

	String* str = (String*)malloc(sizeof(String));
	ASSERT(str, MEMORY_NOT_ASSIGNED_EXCEPTION);

	size_t dim = newString->size;
	str->Buffer = (char*)malloc(dim);
	ASSERT(str->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	memmove(str->Buffer, newString->Buffer, dim);

	str->size = newString->size;

	return str;
}

String String_create_copy(const String* newString)
{
	ASSERT(newString, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(newString->Buffer, MEMORY_POINTING_TO_NOTHING);

	String str;

	size_t dim = newString->size;
	str.Buffer = (char*)malloc(dim);
	ASSERT(str.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);
	memmove(str.Buffer, newString->Buffer, dim);

	str.size = newString->size;

	return str;
}

void String_add(String* const self, const char* string)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);

	size_t sizeOfNewData = strlen(string);

	size_t capacity_in_bytes = self->capacity_in_bytes;
	size_t current_size_in_bytes = self->size;

	if (capacity_in_bytes < current_size_in_bytes + sizeOfNewData + 1)
	{
		capacity_in_bytes += current_size_in_bytes + sizeOfNewData + 16;
		self->capacity_in_bytes = capacity_in_bytes;

		char* tmp = NULL;
		while (tmp == NULL)
			tmp = (char*)realloc(self->Buffer, capacity_in_bytes);
		self->Buffer = tmp;
	}

	ASSERT(self->Buffer, ARRAY_NULL_PASSED_EXCEPTION);

	size_t newSize = self->size + sizeOfNewData;

	const rsize_t destSize = capacity_in_bytes;
	const rsize_t srcSize = sizeOfNewData + 1;

	if (self->size == 0)
		memcpy_s(self->Buffer, destSize, string, srcSize);
	else
		strcat_s(self->Buffer, destSize, string);

	self->size = newSize;
}

void String_emplace(String* const self, int strings, ...)
{
	va_list args;
	va_start(args, strings);

	for (int i = 0; i < strings; i++)
	{
		char* string = (char*)va_arg(args, size_t);
		String_add(self, string);
	}

	va_end(args);
}

void String_delete(String* const self)
{
	if (self != NULL)
	{
		if (self->Buffer != NULL)
		{
			free(self->Buffer);
			self->Buffer = NULL;
		}
		free((String*)self);
	}
	else
		free((String*)self);
}

void String_delete_stacked(String* const self)
{
	if (self != NULL)
	{
		free(self->Buffer);
		self->Buffer = NULL;
	}
}

inline char String_get(const String * self, size_t index)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(index < self->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	return self->Buffer[index];
}

inline void String_set(String* const self, size_t index, char val)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(index < self->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	self->Buffer[index] = val;
}

void String_set_another_String(String* const dest, const String * src)
{
	ASSERT(dest, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(src, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(dest->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(src->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(dest->size == src->size, ARRAY_INVALID_DIMS_EXCEPTION);

	dest->size = src->size;
	dest->capacity_in_bytes = src->capacity_in_bytes;

	memmove(dest->Buffer, src->Buffer, dest->size);
}

String* const String_int_to_string(size_t num)
{
	String* numString;
	char output[100];

	snprintf(output, 100, "%zu", num);

	numString = String_new(strlen(output) + 1);
	memmove(numString->Buffer, output, strlen(output) + 1);

	return numString;
}

String String_int_to_string_stacked(size_t num)
{
	char output[100];

	snprintf(output, 100, "%zu", num);

	String numString = String_create(strlen(output) + 1);
	memmove(numString.Buffer, output, strlen(output) + 1);

	return numString;
}

String* const String_double_to_string(double num)
{
	String* numString;
	char output[100];

	snprintf(output, 100, "%.17lf", num);

	numString = String_new(strlen(output) + 1);
	memmove(numString->Buffer, output, strlen(output) + 1);

	return numString;
}

String String_double_to_string_stacked(double num)
{
	String numString;
	char output[100];

	snprintf(output, 100, "%.17lf", num);

	numString = String_create(strlen(output) + 1);
	memmove(numString.Buffer, output, strlen(output) + 1);

	return numString;
}

void String_print(const String * str)
{
	printf("%s", str->Buffer);
}
