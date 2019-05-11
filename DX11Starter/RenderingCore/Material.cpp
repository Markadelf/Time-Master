#include "Material.h"

Material::Material()
{
	pixelShaderHandle = -1;
	vertexShaderHandle = -1;
	diffuseTextureHandle = -1;
	samplerHandle = -1;
	normalMapHandle = -1;
}

Material::Material(int vertexHandle, int pixelHandle, int diffuseTextureHandle, int roughnessTextureHandle, int samplerHandle, float shinniness, int normalMapHandle)
{
	this->vertexShaderHandle = vertexHandle;
	this->pixelShaderHandle = pixelHandle;
	this->diffuseTextureHandle = diffuseTextureHandle;
	this->samplerHandle = samplerHandle;
	this->roughnessTextureHandle = roughnessTextureHandle;
	this->shinniness = shinniness;
	this->normalMapHandle = normalMapHandle;
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

int Material::GetNormalMapHandle()
{
	return normalMapHandle;
}