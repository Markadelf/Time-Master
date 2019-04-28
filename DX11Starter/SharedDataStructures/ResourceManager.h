#pragma once
#include <unordered_map>
#include "Mesh.h"
#include <string.h>

// Currently, handles are simply indices in order, if I were making a more robust 
// Manager I would permit unloading of Meshes and a stack to keep track
// of which indices are available

template <class T>
class ResourceManager
{
	std::unordered_map<std::string, int> handleMap;
	T* resourceList;

	int count;
	int size;


public:
	ResourceManager(): ResourceManager(32) {
		
	}

	ResourceManager(int size) {
		resourceList = new T[size];
		count = 0;
		this->size = size;
	}

	~ResourceManager() {
		delete[] resourceList;
	}

	// Adds a resource to the dictionary by handle, by default will not overwrite a current filled handle
	int AddResource(std::string name, T resource, bool forceOverwrite = false) {
		auto item = handleMap.find(name);
		if (item == handleMap.end())
		{
			int index = count++;
			handleMap[name] = index;
			resourceList[index] = (resource);
			return index;
		}
		else if (forceOverwrite)
		{
			resourceList[item->second] = resource;
		}
		return item->second;
	}

	// Returns handle to the resource, -1 if not present
	int GetHandle(std::string name) {
		auto item = handleMap.find(name);
		if (item == handleMap.end())
		{
			return -1;
		}
		return item->second;
	}

	// Returns nullptr if name is unknown
	T* GetResourcePointer(std::string name) {
		auto item = handleMap.find(name);
		if (item != handleMap.end())
		{
			return &resourceList[item->second];
		}
		return nullptr;
	}
	
	// May throw an exception if a bad handle is used
	T* GetResourcePointer(int handle) {
		return &resourceList[handle];
	}

	// DANGER ZONE
	// Rather than implement a wrapper for a bunch of different types of pointers/references,
	// I decided to have multiple release functions in my generic

	// This will manually release all of the resources in the system
	void Release() {
		count = 0;
	}

	// This will assume the data is stored as pointers and attempt to deallocate them
	void ReleasePointers() {
		for (size_t i = 0; i < count; i++)
		{
			delete resourceList[i];
		}
		count = 0;
	}

	// This will assume the data is a directX reference and release its references
	void ReleaseDXPointers() {
		for (size_t i = 0; i < count; i++)
		{
			resourceList[i]->Release();
		}
		count = 0;
	}
};

