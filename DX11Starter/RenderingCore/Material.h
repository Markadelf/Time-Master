#pragma once
// A class used to store rendering settings.
// Currently, only the texture is used, but we may want to update the functionality
// Such an update would likely want to use a "if -1 use default" setting
class Material
{
	int pixelShaderHandle;
	int vertexShaderHandle;
	
	int samplerHandle;
	int shinniness;
	int diffuseTextureHandle;
	int roughnessTextureHandle;

public:
	Material();
	Material(int vertexHandle, int pixelHandle, int diffuseTextureHandle,int roughnessTextureHandle, int samplerHandle, float shinniness);
	~Material();

	int GetPixelShaderHandle();
	int GetVertexShaderHandle();
	int GetDiffuseTextureHandle();
	int GetRoughnessTextureHandle();
	float GetShinniness();
	int GetSamplerHandle();
};
