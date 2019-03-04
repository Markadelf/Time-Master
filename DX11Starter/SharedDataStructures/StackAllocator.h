#pragma once
#include <stdlib.h>

class StackAllocator
{
	void* start;
	void* current;
	int size;

public:
	StackAllocator() : StackAllocator(256) {

	}

	StackAllocator(int size) {
		start = malloc(size);
		current = start;
		this->size = size;
	}

	~StackAllocator() {
		free(start);
	}

private:
	void* Allocate(size_t size, __int8 alignment) {
		void* node = current;
		__int8 adjustment = alignment - reinterpret_cast<__int64>(node) % alignment;
		node = (__int8*)node + adjustment;
		*((__int8*)node - 1) = adjustment;

		// Check space
		if ((__int8*)current + size >= (__int8*)start + this->size) {
			return NULL;
		}
		
		current = (__int8*)node + size;
		return node;
	}

	void Deallocate(void* obj) {
		current = reinterpret_cast<__int8*> (obj) - *(reinterpret_cast<__int8*> (obj) - 1);
	}

public:
	template <class T> T* Allocate() {
		// Check for space
		return new (Allocate(sizeof(T), __alignof(T))) T();
	}
	template <class T> T* Allocate(T &other)  {
		// Check for space
		return new (Allocate(sizeof(T), __alignof(T)))T(other);
	}

	template <class T> void Deallocate(T* obj) {
		obj->~T();
		Deallocate((void*) obj);
	}

	template <class T> T* AllocateArray(size_t length) {
		int headerSize = sizeof(size_t) / sizeof(T);

		if (sizeof(size_t) % sizeof(T) > 0)
		{
			headerSize += 1;
		}

		T* p = ((T*)Allocate((length + headerSize) * sizeof(T), __alignof(T))) + headerSize;
		*reinterpret_cast<size_t*>(p - 1) = length;
		
		for (size_t i = 0; i < length; i++)
		{
			new (&p) T;
		}

		return p;
	}

	template<class T> void DeallocateArray(T* array)
	{
		size_t length = *reinterpret_cast<size_t*>(array - 1);
		for (size_t i = 0; i < length; i++)
		{
			array->~T();
		}

		int headerSize = sizeof(size_t) / sizeof(T);

		if (sizeof(size_t) % sizeof(T) > 0)
		{
			headerSize += 1;
		}

		Deallocate((void*)(array - headerSize));
	}

	void Reset() {
		current = start;
	}
};

