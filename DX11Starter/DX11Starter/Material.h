#pragma once
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

