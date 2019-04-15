#pragma once
// A class used to store rendering settings.
// Currently, only the texture is used, but we may want to update the functionality
// Such an update would likely want to use a "if -1 use default" setting
class Material
{
	int pixelShaderHandle;
	int vertexShaderHandle;
	int textureHandle;
	int samplerHandle;

public:
	Material();
	Material(int vertexHandle, int pixelHandle, int textureHandle, int samplerHandle);
	~Material();

	int GetPixelShaderHandle();
	int GetVertexShaderHandle();
	int GetTextureHandle();
	int GetSamplerHandle();
};

