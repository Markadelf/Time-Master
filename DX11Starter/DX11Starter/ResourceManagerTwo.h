#pragma once
// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY
#include"Mesh.h"
#include "FilePathHelper.h"
#include "ResourceManager.h"


class ResourceManagerTwo
{
public:
	static ResourceManagerTwo& get()
	{
		static ResourceManagerTwo sSingleton;
		return sSingleton;
	}

	ResourceManagerTwo();

public:

	void start();

 int LoadMesh(const char * file,ID3D11Device* &DevicePointer);
 Mesh* GetMeshPointer(int EntityMeshHandle);
 int GetMeshHandle(const char* file);

	//ID3D11Device* device1=DXCore::DXCoreInstance;

	void end();
	//struct GetLoadedMeshInfo();
	~ResourceManagerTwo();

private:
		// Managers
		ResourceManager<Mesh> meshManager;
};



