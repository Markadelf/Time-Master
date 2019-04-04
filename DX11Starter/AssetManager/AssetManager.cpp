#include "AssetManager.h"

using namespace DirectX;


AssetManager::AssetManager()
{

}

//Load Material. vHandle- vertex shader handle, pHandle- Pixelshader handle, handle name, where it is string)
void AssetManager::LoadMaterial(int vHandle, int pHandle,const char* filpathname1, const char* filpathname2)
{
	materialManager.AddResource(filpathname1, Material(vHandle, pHandle, textureManager.GetHandle(filpathname2), 0));
}

//Pass string "handle name" as argument. Return int for that handle. 
int AssetManager::GetMaterialHandle(const char * file)
{
	return materialManager.GetHandle(file);
}

//Load Texture. wstring is wide character for file path, later converted to string in function for storing in handle. 
//2nd & 3rd arguments are device and context pointers.
void AssetManager::LoadTexture(std::wstring const file, ID3D11Device* &device, ID3D11DeviceContext* &context)
{
	ID3D11ShaderResourceView* image;
	std::wstring ConvertWStringToString(file);
	std::string FilePathString(ConvertWStringToString.begin(), ConvertWStringToString.end());
	// Add if successful
	if (CreateWICTextureFromFile(device, context, FilePathHelper::GetPath(file).c_str(), 0, &image) == 0)
		textureManager.AddResource(FilePathString, image);
}

//Pass string "handle name" as argument. Return int for that handle. 
int AssetManager::GetTextureHandle(const char * file)
{
	return textureManager.GetHandle(file);
}

//Load Mesh. Calls Mesh class and sets vertex and index GPU buffers. Draw calls are called later. 
int AssetManager::LoadMesh(const char * file,ID3D11Device* &DevicePointer)
{	
	return meshManager.AddResource(file,Mesh(file,DevicePointer));
}

//Pass string "handle name" as argument. Return int for that handle. 
int AssetManager::GetMeshHandle(const char * file)
{
	return meshManager.GetHandle(file);
}

AssetManager::~AssetManager()
{
	meshManager.Release();
	materialManager.Release();
	textureManager.ReleaseDXPointers();
}


