#pragma once
// NOT UNNECESSARY
#include "Winsock2.h"
// NOT UNNECESSARY
#include"Mesh.h"
#include "Emitter.h"
#include "FilePathHelper.h"
#include "AssetManager.h"
#include "ResourceManager.h"
#include "Material.h"

#include "WICTextureLoader.h"



class AssetManager
{
public:
	//singleton for AssetManger and use by returning it in other classes. Static variable is constructed on first invocation of it's call rather than before "main". Return reference to the object and can be used as object variable in other classes.
	static AssetManager& get()
	{
		static AssetManager sSingleton;
		return sSingleton;
	}

	AssetManager();

public:
	//Load Material. vHandle- vertex shader handle, pHandle- Pixelshader handle, handle name, where it is string)
	void LoadMaterial(int vHandle, int pHandle, const char* materailName, const char* texture_albedo_filepath, const char* texture_roughness_filepath);
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

	int GetEmitterHandle(const char * file);

	void ReleaseAllAssetResource();

	void LoadEmitter(const char* file, int maxParticles, int pPerSec, float pLifetime, float pStartSize, float pEndSize, XMFLOAT4 pStartColor, XMFLOAT4 pEndColor, XMFLOAT3 pStartVelocity, XMFLOAT3 pVelocityRandRange,XMFLOAT3 emitterPos, XMFLOAT3 emitterPosRandRange, XMFLOAT4 emitterRotRandRange, XMFLOAT3 pConstAccel, ID3D11Device* device, int textureHandle);

	~AssetManager();

	//Emitter* m_

	// Inlined template functions for taking string and int data types. Avoid duplicacy from other classes and linker problems. Refer ResourceManager.h
	template <typename T>
	Mesh* GetMeshPointer(T t);
	// Inlined template functions for taking string and int data types. Avoid duplicacy from other classes and linker problems. Refer ResourceManager.h
	template <typename T>
	ID3D11ShaderResourceView** GetTexturePointer(T t);
	// Inlined template functions for taking string and int data types. Avoid duplicacy from other classes and linker problems. Refer ResourceManager.h
	template <typename T>
	Material* GetMaterialPointer(T t);

	template <typename T>
	Emitter* GetEmitterPointer(T t);

private:
		// Managers
		ResourceManager<Mesh> meshManager;
		ResourceManager<Material> materialManager;
		ResourceManager<ID3D11ShaderResourceView*> textureManager;
		ResourceManager<Emitter*> emitterManager;
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

template<typename T>
inline Emitter* AssetManager::GetEmitterPointer(T t)
{
	return *emitterManager.GetResourcePointer(t);
}
