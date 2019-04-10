#include "Material.h"

Material::Material()
{
	pixelShaderHandle = -1;
	vertexShaderHandle = -1;
	diffuseTextureHandle = -1;
	samplerHandle = -1;
}

Material::Material(int vertexHandle, int pixelHandle, int diffuseTextureHandle, int roughnessTextureHandle, int samplerHandle, float shinniness)
{
	this->vertexShaderHandle = vertexHandle;
	this->pixelShaderHandle = pixelHandle;
	this->diffuseTextureHandle = diffuseTextureHandle;
	this->samplerHandle = samplerHandle;
	this->roughnessTextureHandle = roughnessTextureHandle;
	this->shinniness = shinniness;
}

Material::~Material()
{
}

int Material::GetPixelShaderHandle()
{
	return vertexShaderHandle;
}

int Material::GetVertexShaderHandle()
{
	return pixelShaderHandle;
}

int Material::GetDiffuseTextureHandle()
{
	return diffuseTextureHandle;
}

int Material::GetRoughnessTextureHandle()
{
	return roughnessTextureHandle;
}

int Material::GetSamplerHandle()
{
	return samplerHandle;
}

float Material::GetShinniness()
{
	return shinniness;
}