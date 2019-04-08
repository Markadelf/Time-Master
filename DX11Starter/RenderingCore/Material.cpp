#include "Material.h"

Material::Material()
{
	pixelShaderHandle = -1;
	vertexShaderHandle = -1;
	textureHandle = -1;
	samplerHandle = -1;
}

Material::Material(int vertexHandle, int pixelHandle, int textureHandle, int samplerHandle)
{
	this->vertexShaderHandle = vertexHandle;
	this->pixelShaderHandle = pixelHandle;
	this->textureHandle = textureHandle;
	this->samplerHandle = samplerHandle;
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

int Material::GetTextureHandle()
{
	return textureHandle;
}

int Material::GetSamplerHandle()
{
	return samplerHandle;
}
