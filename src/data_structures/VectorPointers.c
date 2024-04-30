#include "VectorPointers.h"
#include "error_management.h"
#include <stdarg.h>
#include <string.h>
#include "VectorPointers.h"
#include "DetectMemoryLeaks.h"

VectorPointers* VectorPointers_new(void (*deleter)(void* ptr_to_struct))
{
	size_t capacity = 2 * sizeof(void*);
	VectorPointers* newVec = (VectorPointers*)malloc(sizeof(VectorPointers));
	ASSERT(newVec, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec->Buffer = (void**)malloc(capacity);
	ASSERT(newVec->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec->size = 0;
	newVec->capacity_in_bytes = capacity;
	newVec->deleter = deleter;

	return newVec;
}

VectorPointers VectorPointers_create(void(*deleter)(void* ptr_to_struct))
{
	size_t capacity = 2 * sizeof(void*);

	VectorPointers newVec;

	newVec.Buffer = (void**)malloc(capacity);
	ASSERT(newVec.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec.size = 0;
	newVec.capacity_in_bytes = capacity;
	newVec.deleter = deleter;

	return newVec;
}

VectorPointers* VectorPointers_new_init_size(void (*deleter)(void* ptr_to_struct), size_t size)
{
	size_t capacity = (size + 2) * sizeof(void*);
	VectorPointers* newVec = (VectorPointers*)malloc(sizeof(VectorPointers));
	ASSERT(newVec, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec->Buffer = (void**)malloc(capacity);
	ASSERT(newVec->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec->size = 0;
	newVec->capacity_in_bytes = capacity;
	newVec->deleter = deleter;

	return newVec;
}

VectorPointers VectorPointers_create_init_size(void(*deleter)(void* ptr_to_struct), size_t size)
{
	size_t capacity = (size + 2) * sizeof(void*);

	VectorPointers newVec;

	newVec.Buffer = (void**)malloc(capacity);
	ASSERT(newVec.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec.size = 0;
	newVec.capacity_in_bytes = capacity;
	newVec.deleter = deleter;

	return newVec;
}

VectorPointers* VectorPointers_new_init(void (*deleter)(void* ptr_to_struct), size_t size, ...)
{
	size_t capacity = (size + 2) * sizeof(void*);
	VectorPointers* newVec = (VectorPointers*)malloc(sizeof(VectorPointers));
	ASSERT(newVec, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec->Buffer = (void**)malloc(capacity);
	ASSERT(newVec->Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec->size = size;
	newVec->capacity_in_bytes = capacity;
	newVec->deleter = deleter;

	va_list args;
	va_start(args, size);

	for (size_t i = 0; i < size; i++)
	{
		size_t elem_pointer = va_arg(args, size_t);
		memmove(newVec->Buffer + i, &elem_pointer, sizeof(void*));
	}

	va_end(args);

	return newVec;
}

VectorPointers VectorPointers_create_init(void(*deleter)(void* ptr_to_struct), size_t size, ...)
{
	size_t capacity = (size + 2) * sizeof(void*);

	VectorPointers newVec;

	newVec.Buffer = (void**)malloc(capacity);
	ASSERT(newVec.Buffer, MEMORY_NOT_ASSIGNED_EXCEPTION);

	newVec.size = size;
	newVec.capacity_in_bytes = capacity;
	newVec.deleter = deleter;

	va_list args;
	va_start(args, size);

	for (size_t i = 0; i < size; i++)
	{
		size_t elem_pointer = va_arg(args, size_t);
		memmove(newVec.Buffer + i, &elem_pointer, sizeof(void*));
	}

	va_end(args);

	return newVec;
}

void VectorPointers_delete(VectorPointers* self)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);

	for (size_t i = 0; i < self->size; i++)
	{
		void* pointer = self->Buffer[i];

		if (self->deleter != NULL)
			self->deleter(pointer);
		else
			free(pointer);
	}

	free(self->Buffer);
	self->Buffer = NULL;
	free(self);
	self = NULL;
}

void VectorPointers_delete_stacked(VectorPointers* self)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);

	for (size_t i = 0; i < self->size; i++)
	{
		void* pointer = self->Buffer[i];

		if (self->deleter != NULL)
			self->deleter(pointer);
		else
			free(pointer);
	}

	free(self->Buffer);
	self->Buffer = NULL;
	self = NULL;
}

void VectorPointers_add(VectorPointers* self, void * newPointer)
{
	ASSERT(self, ARRAY_NULL_PASSED_EXCEPTION);
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);
	ASSERT(newPointer, MEMORY_POINTING_TO_NOTHING);

	size_t capacity_in_bytes = self->capacity_in_bytes;
	size_t current_size_in_bytes = self->size * sizeof(void*);

	if (capacity_in_bytes < current_size_in_bytes + sizeof(void*))
	{
		capacity_in_bytes += 2 * sizeof(void*);
		self->capacity_in_bytes = capacity_in_bytes;

		void** tmp = NULL;
		while (tmp == NULL)
			tmp = (void**)realloc(self->Buffer, capacity_in_bytes);
		self->Buffer = tmp;
	}

	size_t newSize = self->size;
	ASSERT(self->Buffer, MEMORY_POINTING_TO_NOTHING);
	self->Buffer[newSize++] = newPointer;
	self->size = newSize;
}

void VectorPointers_emplace(VectorPointers* self, size_t quantity, ...)
{
	ASSERT(self, MEMORY_NOT_ASSIGNED_EXCEPTION);

	va_list args;
	va_start(args, quantity);

	for (size_t i = 0; i < quantity; i++)
	{
		size_t layer_pointer = va_arg(args, size_t);
		VectorPointers_add(self, &layer_pointer);
	}

	va_end(args);
}

void* VectorPointers_get(VectorPointers* self, size_t index)
{
	ASSERT(index < self->size, ARRAY_OUT_OF_BOUNDS_EXCEPTION);

	return self->Buffer[index];
}
