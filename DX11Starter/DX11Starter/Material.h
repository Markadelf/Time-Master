#pragma once
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

