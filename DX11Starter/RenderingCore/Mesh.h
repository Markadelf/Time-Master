#pragma once
#include <d3d11.h>
#include "Vertex.h"
#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"

// Acts as a smart pointer for the DirectX Pointers
class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	int indexCount;

public:
	Mesh();
	// Constructor used to properly initialize the data
	Mesh(Vertex* buffer, int vCount, int* indices, int iCount, ID3D11Device* device);
	Mesh(const char* file, ID3D11Device* device);
	// Constructors to handle handling cases without screwing up the ref count
	Mesh(const Mesh &other);
	Mesh& Mesh::operator= (const Mesh&);
	Mesh& operator= (Mesh&&);

	~Mesh();
	
	// Accessor functions
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

private:
	void Init(Vertex* buffer, int vCount, UINT* indices, int iCount, ID3D11Device* device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

};

