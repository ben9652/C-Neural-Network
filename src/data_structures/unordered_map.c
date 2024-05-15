#include "unordered_map.h"
#include "error_management.h"
#include "Vector.h"
#include "Random.h"
#include <math.h>

#define INITIAL_CAPACITY 3

typedef double type_value;

typedef struct entry
{
	uint8_t* key;
	type_value value;
	void* object;
	struct entry* next;
} Entry;

typedef struct _hash_table
{
	size_t size;
	size_t capacity;
	hashfunction* hash;
	size_t keyLength;
	Entry** elements;

	// Puntero a una función destructora de los objetos que se guarden
	void (*deleter)(void*);

	// Para guardar los bytes cuyos bits serán invertidos
	bool* bytes_to_invert;

	// Número primo que se usará durante la existencia de este mapa para obtener el índice
	size_t prime_number;

	bool thereAreStoredObjects;
} unordered_map;

static bool isPrime(size_t n)
{
	if (n <= 1)
		return false;
	if (n == 2 || n == 3)
		return true;
	if (n % 2 == 0 || n % 3 == 0)
		return false;

	for (size_t i = 5; i <= (size_t)trunc(sqrt((double)n)); i += 6)
	{
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	}

	return true;
}

static size_t prevPrime(size_t n)
{
	if (n <= 1)
		return 2;
	size_t prime = n;
	bool found = false;

	while (!found)
	{
		prime--;
		if (isPrime(prime))
			found = true;
	}

	return prime;
}

static uint64_t hash(unordered_map* um, const uint8_t* data, size_t length)
{
	unsigned int hash_value = 0;
	for (size_t i = 0; i < length; i++)
	{
		uint8_t thisByte = data[i];
		// Si el byte direccionado tiene la orden de que se le inviertan los bits, se los invierten.
		// Esto se hace más que nada para que varíen un poco los bytes y que no se compute el hash simplemente con la clave plana.
		if (um->bytes_to_invert[i])
			thisByte ^= 0xFF;
		hash_value += thisByte;
	}
	return hash_value;
}

static size_t hashtable_index(unordered_map* self, const void* key)
{
	size_t result = self->hash(self, (const uint8_t*)key, self->keyLength) % self->prime_number;
	return result;
}

unordered_map* unordered_map_new_store_objects(size_t keylen, void (*deleter)(void* obj))
{
	unordered_map* newUM = (unordered_map*)malloc(sizeof(unordered_map));
	ASSERT(newUM, MEMORY_NOT_ASSIGNED_EXCEPTION);
	newUM->size = 0;
	newUM->capacity = keylen * 0xFF / 2;
	newUM->hash = hash;
	newUM->elements = (Entry**)calloc(newUM->capacity, sizeof(Entry*));
	newUM->keyLength = keylen;

	newUM->prime_number = prevPrime(keylen * 0xFF / 2);
	newUM->deleter = deleter;

	newUM->thereAreStoredObjects = true;

	// Creo el vector de bytes que serán invertidos
	newUM->bytes_to_invert = (uint8_t*)malloc(keylen);
	ASSERT(newUM->bytes_to_invert, MEMORY_NOT_ASSIGNED_EXCEPTION);

	int randInt;
	for (size_t i = 0; i < keylen; i++)
	{
		randInt = genRandomInt(0, 3);
		newUM->bytes_to_invert[i] = randInt ? false : true;
	}

	return newUM;
}

unordered_map* unordered_map_new_store_values(size_t keylen)
{
	unordered_map* newUM = (unordered_map*)malloc(sizeof(unordered_map));
	ASSERT(newUM, MEMORY_NOT_ASSIGNED_EXCEPTION);
	newUM->size = 0;
	newUM->capacity = keylen * 0xFF / 2;
	newUM->hash = hash;
	newUM->elements = (Entry**)calloc(newUM->capacity, sizeof(Entry*));
	newUM->keyLength = keylen;

	newUM->prime_number = prevPrime(keylen * 0xFF / 2);

	newUM->thereAreStoredObjects = false;

	// Creo el vector de bytes que serán invertidos
	newUM->bytes_to_invert = (uint8_t*)malloc(keylen);
	ASSERT(newUM->bytes_to_invert, MEMORY_NOT_ASSIGNED_EXCEPTION);

	int randInt;
	for (size_t i = 0; i < keylen; i++)
	{
		randInt = genRandomInt(0, 3);
		newUM->bytes_to_invert[i] = randInt ? false : true;
	}

	return newUM;
}

static void delete_entry(Entry* entry, void (*deleter)(void*))
{
	if (entry)
	{
		if(entry->object != NULL)
		{
			if (deleter)
				deleter(entry->object);
			else
				free(entry->object);
		}

		free(entry->key);

		if (entry->next)
			delete_entry(entry->next, deleter);

		free(entry);
	}
}

void unordered_map_delete(unordered_map* self)
{
	if (self != NULL)
	{
		if (self->elements != NULL)
		{
			for (size_t i = 0; i < self->capacity; i++)
			{
				if (self->elements[i] != NULL)
					delete_entry(self->elements[i], self->deleter);
			}
			free(self->elements);
		}
		free(self->bytes_to_invert);
		free(self);
	}
}

static void clear_entry(Entry* entry, size_t keylen, void (*deleter)(void*))
{
	if (entry)
	{
		if (entry->object != NULL)
		{
			if (deleter)
				deleter(entry->object);
			else
				free(entry->object);
		}
		else
			memset(entry->key, 0, keylen);
	}
}

void unordered_map_clear(unordered_map* self)
{
	if (self != NULL)
	{
		if (self->elements != NULL)
		{
			for (size_t i = 0; i < self->capacity; i++)
			{
				if (self->elements[i] != NULL)
					clear_entry(self->elements[i], self->keyLength, self->deleter);
			}
		}
	}
	self->size = 0;
}

void unordered_map_print(unordered_map* self)
{
	printf("Start map\n");
	for (uint32_t i = 0; i < self->capacity; i++)
	{
		if (self->elements[i] == NULL)
			printf("\t%d\t---\n", i);
		else
		{
			printf("\t%d\t\n", i);
			Entry* tmp = self->elements[i];
			while (tmp != NULL)
			{
				printf("\"0x");
				for (size_t c = self->keyLength - 1; c != 0; c--)
					printf("%02x", tmp->key[c]);
				printf("%02x\" (0x%p) - ", tmp->key[0], tmp->object);
				tmp = tmp->next;
			}
			printf("\n");
		}
	}
}

bool unordered_map_insert(unordered_map* self, const void* key, void* obj)
{
	if (key == NULL || obj == NULL) return false;
	size_t index = hashtable_index(self, key);

	if (unordered_map_lookup(self, key) != NULL) return false;

	Entry* e = (Entry*)malloc(sizeof(Entry));
	ASSERT(e, MEMORY_NOT_ASSIGNED_EXCEPTION);
	if (self->thereAreStoredObjects)
		e->object = obj;
	else
	{
		type_value* value = obj;
		e->value = *value;
		e->object = NULL;
	}
	e->key = (uint8_t*)malloc(self->keyLength);
	memmove(e->key, key, self->keyLength);

	self->size++;

	e->next = self->elements[index];
	if (index + 1 > self->capacity || self->size > self->capacity)
	{
		self->capacity = self->capacity * 3 / 2;
		Entry** tmp = NULL;
		while (tmp == NULL)
			tmp = (Entry**)realloc(self->elements, self->capacity * sizeof(Entry*));
		Entry* not_assigned_address = (Entry*)0xcdcdcdcdcdcdcdcd;
		for (size_t i = 0; i < self->capacity; i++)
		{
			if (tmp[i] == not_assigned_address)
				tmp[i] = 0;
		}
		self->elements = tmp;
	}

	if (self->elements)
	{
		self->elements[index] = e;
		return true;
	}
	else
		return false;
}

void* unordered_map_lookup(unordered_map* self, const void* key)
{
	if (key == NULL || self == NULL) return false;
	size_t index = hashtable_index(self, key);

	if (index + 1 > self->capacity) return NULL;

	Entry* tmp = self->elements[index];
	while (tmp != NULL && memcmp(tmp->key, key, self->keyLength) != 0)
		tmp = tmp->next;

	if (tmp == NULL) return NULL;

	return self->thereAreStoredObjects ? tmp->object : &tmp->value;
}

void* unordered_map_erase(unordered_map* self, const void* key)
{
	if (key == NULL || self == NULL) return false;
	key = (uint8_t*)key;
	size_t index = hashtable_index(self, key);

	Entry* tmp = self->elements[index];
	Entry* prev = NULL;
	while (tmp != NULL && memcmp(tmp->key, key, self->keyLength) != 0)
	{
		prev = tmp;
		tmp = tmp->next;
	}

	if (tmp == NULL) return NULL;

	if (prev == NULL)
		self->elements[index] = tmp->next;
	else
		prev->next = tmp->next;

	void* result = tmp->object;
	free(tmp);

	return result;
}

size_t unordered_map_size(unordered_map* self)
{
	return self->size;
}

static void unordered_map_print_strings(unordered_map* self)
{
	printf("Start map\n");
	for (uint32_t i = 0; i < self->capacity; i++)
	{
		if (self->elements[i] == NULL)
			printf("\t%d\t---\n", i);
		else
		{
			printf("\t%d\t\n", i);
			Entry* tmp = self->elements[i];
			while (tmp != NULL)
			{
				printf("\"0x");
				for (size_t c = self->keyLength - 1; c != 0; c--)
					printf("%02x", tmp->key[c]);
				printf("%02x\" (%s) - ", tmp->key[0], (char*)tmp->object);
				tmp = tmp->next;
			}
			printf("\n");
		}
	}
}

void unordered_map_test()
{
#pragma region declaring_keys
	Matrix_Position  pos1 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos2 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos3 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos4 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos5 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos6 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos7 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos8 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position  pos9 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos10 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos11 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos12 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos13 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos14 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos15 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos16 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos17 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos18 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos19 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos20 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos21 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos22 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos23 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos24 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos25 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
	Matrix_Position pos26 = M_Pos(genRandomInt(0, 200), genRandomInt(0, 200));
#pragma endregion

#pragma region declaring_objects
	char* obj1 = "A";
	char* obj2 = "B";
	char* obj3 = "C";
	char* obj4 = "D";
	char* obj5 = "E";
	char* obj6 = "F";
	char* obj7 = "G";
	char* obj8 = "H";
	char* obj9 = "I";
	char* obj10 = "J";
	char* obj11 = "K";
	char* obj12 = "L";
	char* obj13 = "M";
	char* obj14 = "N";
	char* obj15 = "O";
	char* obj16 = "P";
	char* obj17 = "Q";
	char* obj18 = "R";
	char* obj19 = "S";
	char* obj20 = "T";
	char* obj21 = "U";
	char* obj22 = "V";
	char* obj23 = "W";
	char* obj24 = "X";
	char* obj25 = "Y";
	char* obj26 = "Z";
#pragma endregion

	unordered_map* map = unordered_map_new_store_objects(sizeof(Matrix_Position), NULL);
	unordered_map_insert(map, &pos1, obj1);
	unordered_map_insert(map, &pos2, obj2);
	unordered_map_insert(map, &pos3, obj3);
	unordered_map_insert(map, &pos4, obj4);
	unordered_map_insert(map, &pos2, obj3);
	unordered_map_insert(map, &pos5, obj5);
	unordered_map_insert(map, &pos6, obj6);
	unordered_map_insert(map, &pos7, obj7);
	unordered_map_insert(map, &pos8, obj8);
	unordered_map_insert(map, &pos9, obj9);
	unordered_map_insert(map, &pos10, obj10);
	unordered_map_insert(map, &pos11, obj11);
	unordered_map_insert(map, &pos12, obj12);
	unordered_map_insert(map, &pos13, obj13);
	unordered_map_insert(map, &pos14, obj14);
	unordered_map_insert(map, &pos15, obj15);
	unordered_map_insert(map, &pos16, obj16);
	unordered_map_insert(map, &pos17, obj17);
	unordered_map_insert(map, &pos18, obj18);
	unordered_map_insert(map, &pos19, obj19);
	unordered_map_insert(map, &pos20, obj20);
	unordered_map_insert(map, &pos21, obj21);
	unordered_map_insert(map, &pos22, obj22);
	unordered_map_insert(map, &pos23, obj23);
	unordered_map_insert(map, &pos24, obj24);
	unordered_map_insert(map, &pos25, obj25);
	unordered_map_insert(map, &pos26, obj26);

	char* a = unordered_map_lookup(map, &pos1);
	char* b = unordered_map_lookup(map, &pos2);
	char* c = unordered_map_lookup(map, &pos3);
	char* d = unordered_map_lookup(map, &pos4);
	char* e = unordered_map_lookup(map, &pos5);
	char* f = unordered_map_lookup(map, &pos6);

	unordered_map_print_strings(map);

	unordered_map_delete(map);
}
