#pragma once
#include <stdlib.h>

template<__int8 nodeSize>// where nodeSize: >= 8
class PoolAllocator
{
	void* start;
	void* freeNode;
	int nodeCount;

public:
	PoolAllocator() : PoolAllocator(256) {

	}

	PoolAllocator(int size) {
		// Can't support data smaller than pointer size
		if (nodeSize < sizeof(void*)) {
			throw ERROR_INVALID_PARAMETER;
		}
		
		start = malloc((nodeCount + 1) * nodeSize);

		// Allign
		__int8 adjustment = nodeSize - reinterpret_cast<__int64>(start) % nodeSize;
		start = (__int8*)start + adjustment;
		*((__int8*)start - 1) = adjustment;
		
		freeNode = start;
		nodeCount = size;

		__int8* iterator = reinterpret_cast<__int8*>(start);
		for (size_t i = 0; i < nodeCount - 1; i++)
		{
			*reinterpret_cast<void**>(iterator) = iterator + nodeSize;
			iterator += nodeSize;
		}
		*reinterpret_cast<void**>(iterator) = nullptr;
	}

	~PoolAllocator() {
		free(reinterpret_cast<__int8*> (start) - *(reinterpret_cast<__int8*> (start) - 1));
	}

private:
	void* Allocate() {
		// Check space
		if (freeNode == nullptr) {
			throw ERROR_OUTOFMEMORY;
		}
		
		void* node = freeNode;

		freeNode = *(reinterpret_cast<void**>(node));

		return node;
	}

	void Deallocate(void* obj) {
		*reinterpret_cast<void**>(obj) = freeNode;
		freeNode = obj;
	}

public:
	template <class T> T* Allocate() {
		if (nodeSize < sizeof(T)) {
			throw ERROR_INVALID_PARAMETER;
		}
		return new (Allocate()) T();
	}
	template <class T> T* Allocate(T &other)  {
		if (nodeSize < sizeof(T)) {
			throw ERROR_INVALID_PARAMETER;
		}
		return new (Allocate()) T(other);
	}

	template <class T> void Deallocate(T* obj) {
		obj->~T();
		Deallocate((void*) obj);
	}
};

