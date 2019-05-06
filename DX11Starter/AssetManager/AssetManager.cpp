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

int AssetManager::GetEmitterHandle(const char * file)
{
	return emitterManager.GetHandle(file);
}
void AssetManager::ReleaseAllAssetResource()
{
	meshManager.Release();
	materialManager.Release();
	textureManager.ReleaseDXPointers();
    emitterManager.ReleasePointers();
}

void AssetManager::LoadEmitter(const char* file,int maxParticles, int pPerSec, float pLifetime, float pStartSize, float pEndSize, XMFLOAT4 pStartColor, XMFLOAT4 pEndColor, XMFLOAT3 pStartVelocity, XMFLOAT3 pVelocityRandRange, XMFLOAT3 emitterPos, XMFLOAT3 emitterPosRandRange, XMFLOAT4 emitterRotRandRange, XMFLOAT3 pConstAccel, ID3D11Device* device, int textureHandle)
{
	//// Parameters- Max particles, Device, pos, pos rand range, start color, end color, 
	//emitter = new Emitter(
	//	maxParticles,							// Max particles
	//	pPerSec,								// Particles per second
	//	pLifetime,							// Particle lifetime
	//	pStartSize,							// Start size
	//	pEndSize,							// End size
	//	pStartColor,	// Start color
	//	pEndColor,		// End color
	//	pStartVelocity,				// Start velocity
	//	pVelocityRandRange,		        // Velocity randomness range
	//	emitterPos,		        // Emitter position
	//	emitterPosRandRange,		// Position randomness range
	//	emitterRotRandRange,			// Random rotation ranges (startMin, startMax, endMin, endMax)
	//	pConstAccel,				// Constant acceleration
	//	device,
	//	textureHandle);

	//// Parameters- Max particles, Device, pos, pos rand range, start color, end color, 
	Emitter* emitter = new Emitter(
		maxParticles,							// Max particles
		pPerSec,								// Particles per second
		pLifetime,							// Particle lifetime
		pStartSize,							// Start size
		pEndSize,							// End size
		pStartColor,	// Start color
		pEndColor,		// End color
		pStartVelocity,				// Start velocity
		pVelocityRandRange,		        // Velocity randomness range
		emitterPos,		        // Emitter position
		emitterPosRandRange,		// Position randomness range
		emitterRotRandRange,			// Random rotation ranges (startMin, startMax, endMin, endMax)
		pConstAccel,				// Constant acceleration
		device,
		textureHandle);

	emitterManager.AddResource(file, emitter);
}
AssetManager::~AssetManager()
{

}


