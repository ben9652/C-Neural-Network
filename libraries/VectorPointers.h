#ifndef _VECTORS_POINTERS
#define _VECTORS_POINTERS

typedef struct
{
	void** Buffer;
	size_t size;
	size_t capacity_in_bytes;
	void (*deleter)(void* ptr_to_struct);
} VectorPointers;

VectorPointers* VectorPointers_new(void (*deleter)(void* ptr_to_struct));
VectorPointers VectorPointers_create(void (*deleter)(void* ptr_to_struct));

VectorPointers* VectorPointers_new_init_size(void (*deleter)(void* ptr_to_struct), size_t size);
VectorPointers VectorPointers_create_init_size(void (*deleter)(void* ptr_to_struct), size_t size);

VectorPointers* VectorPointers_new_init(void (*deleter)(void* ptr_to_struct), size_t size, ...);
VectorPointers VectorPointers_create_init(void (*deleter)(void* ptr_to_struct), size_t size, ...);

void VectorPointers_delete(VectorPointers* self);
void VectorPointers_delete_stacked(VectorPointers* self);

void VectorPointers_add(VectorPointers* self, void * newPointer);
void VectorPointers_emplace(VectorPointers* self, size_t quantity, ...);

void* VectorPointers_get(VectorPointers* self, size_t index);

#endif