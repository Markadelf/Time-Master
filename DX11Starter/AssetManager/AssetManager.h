#pragma once
// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY
#include"Mesh.h"
#include "FilePathHelper.h"
#include "AssetManager.h"
#include "ResourceManager.h"
#include "Material.h"
#include "WICTextureLoader.h"


class AssetManager
{
public:
	//singleton for AssetManger and use by returning it in other classes. Static variable is constructed on first invocation of it's call rather than before "main"
	static AssetManager& get()
	{
		static AssetManager sSingleton;
		return sSingleton;
	}

	AssetManager();

public:
	//Load Material. vHandle- vertex shader handle, pHandle- Pixelshader handle, handle name, where it is string)
	void LoadMaterial(int vHandle, int pHandle, const char* filpathname1, const char* filpathname2);
	//Pass string "handle name" as argument. Return int for that handle. 
	int GetMaterialHandle(const char* file);

	//Load Texture. wstring is wide character for file path, later converted to string in function for storing in handle. 
	//2nd & 3rd arguments are device and context pointers.
	void LoadTexture(std::wstring const file, ID3D11Device* &DevicePointer, ID3D11DeviceContext* &DeviceContextPointer);
	//Pass string "handle name" as argument. Return int for that handle. 
	int GetTextureHandle(const char* file);

	//Load Mesh. Calls Mesh class and sets vertex and index GPU buffers. Draw calls are called later. 
	int LoadMesh(const char* file,ID3D11Device* &DevicePointer);
	//Pass string "handle name" as argument. Return int for that handle. 
	int GetMeshHandle(const char* file);

	void ReleaseAllAssetResource();

	~AssetManager();

	// Inlined template functions for taking string and int data types. Avoid duplicacy from other classes and linker problems. Refer ResourceManager.h
	template <typename T>
	Mesh* GetMeshPointer(T t);
	// Inlined template functions for taking string and int data types. Avoid duplicacy from other classes and linker problems. Refer ResourceManager.h
	template <typename T>
	ID3D11ShaderResourceView** GetTexturePointer(T t);
	// Inlined template functions for taking string and int data types. Avoid duplicacy from other classes and linker problems. Refer ResourceManager.h
	template <typename T>
	Material* GetMaterialPointer(T t);

private:
		// Managers
		ResourceManager<Mesh> meshManager;
		ResourceManager<Material> materialManager;
		ResourceManager<ID3D11ShaderResourceView*> textureManager;
};

template<typename T>
inline Mesh* AssetManager::GetMeshPointer(T t)
{
	return meshManager.GetResourcePointer(t);
}

template<typename T>
inline ID3D11ShaderResourceView** AssetManager::GetTexturePointer(T t)
{
	return textureManager.GetResourcePointer(t);
}

template<typename T>
inline Material* AssetManager::GetMaterialPointer(T t)
{
	return materialManager.GetResourcePointer(t);
}
