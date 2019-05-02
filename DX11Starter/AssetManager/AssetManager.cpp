#include "AssetManager.h"

using namespace DirectX;


AssetManager::AssetManager()
{

}

//Load Material. vHandle- vertex shader handle, pHandle- Pixelshader handle, handle name, where it is string)
void AssetManager::LoadMaterial(int vHandle, int pHandle,const char* materialName, const char* texture_albedo_filepath,const char* texture_roughness_filepath)
{
	materialManager.AddResource(materialName, Material(vHandle, pHandle, textureManager.GetHandle(texture_albedo_filepath), textureManager.GetHandle(texture_roughness_filepath), 0,64.0f));
}

//Pass string "handle name" as argument. Return int for that handle. 
int AssetManager::GetMaterialHandle(const char* file)
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

void AssetManager::ReleaseAllAssetResource()
{
	meshManager.Release();
	materialManager.Release();
	textureManager.ReleaseDXPointers();
}
void AssetManager::LoadEmitter(int maxParticles, int pPerSec, float pLifetime, float pStartSize, float pEndSize, XMFLOAT4 pStartColor, XMFLOAT4 pEndColor, XMFLOAT3 pStartVelocity, XMFLOAT4 pVelocityRandRange, XMFLOAT4 emitterPos, XMFLOAT4 emitterPosRandRange, XMFLOAT4 emitterRotRandRange, XMFLOAT4 pConstAccel, ID3D11Device * device, int pTextureHandle)
{
	// Parameters- Max particles, Device, pos, pos rand range, start color, end color, 
	m_drawInfo.emitter = new Emitter(
		10,							// Max particles
		20,								// Particles per second
		.5f,							// Particle lifetime
		0.1f,							// Start size
		2.0f,							// End size
		XMFLOAT4(1, 0.1f, 0.1f, 0.7f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0),		// End color
		XMFLOAT3(0, 0, 0),				// Start velocity
		XMFLOAT3(1, 1, 1),		        // Velocity randomness range
		XMFLOAT3(0, 0, 0),		        // Emitter position
		XMFLOAT3(0.1f, 0.1f, 0.1f),		// Position randomness range
		XMFLOAT4(-2, 2, -2, 2),			// Random rotation ranges (startMin, startMax, endMin, endMax)
		XMFLOAT3(0, -1, 0),				// Constant acceleration
		device,
		AssetManager::get().GetTextureHandle("Textures/particle.jpg"));

}
AssetManager::~AssetManager()
{

}


