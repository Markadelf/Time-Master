#include "Mesh.h"
#include <fstream>
#include <vector>
#include <DirectXMath.h>
#include "FilePathHelper.h"
#include "assimp/anim.h"

using namespace DirectX;
//using namespace Assimp;

Mesh::Mesh()
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	indexCount = 0;
}

Mesh::Mesh(Vertex* vertices, int vCount, int* indices, int iCount, ID3D11Device* device) : Mesh()
{
	Init(vertices, vCount, reinterpret_cast<UINT*>(indices), iCount, device);
}

Mesh::Mesh(const char* file, ID3D11Device* device) : Mesh()
{
	std::string filePath = FilePathHelper::GetPath(file);


	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	//char chars[100];                     // String for line reading

	Assimp::Importer importer;



	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_FlipUVs);
	aiMesh* mesh = pScene->mMeshes[0];
	
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		
		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

	

			vertex.UV.x = mesh->mTextureCoords[0][i].x;
			vertex.UV.y = mesh->mTextureCoords[0][i].y;


		

		verts.push_back(vertex);

		vertCounter += 1;
		vertCounter += 1;
		vertCounter += 1;
	}

	for (UINT c = 0; c < mesh->mNumFaces; c++)
		for (UINT e = 0; e < mesh->mFaces[c].mNumIndices; e++)
			indices.push_back(mesh->mFaces[c].mIndices[e]);


	
//	vertCounter = mesh->mNumVertices;
	//pScene->~aiScene();
	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the address of the first vert
	//
	// - The vector "indices" is similar. It's a vector of unsigned ints and
	//    can be used directly for the index buffer: &indices[0] is the address of the first int
	//
	// - "vertCounter" is BOTH the number of vertices and the number of indices
	// - Yes, the indices are a bit redundant here (one per vertex).  Could you skip using
	//    an index buffer in this case?  Sure!  Though, if your mesh class assumes you have
	//    one, you'll need to write some extra code to handle cases when you don't.

	Init(&verts[0], vertCounter, &indices[0], vertCounter, device);
}

Mesh::Mesh(const Mesh & other)
{
	if (vertexBuffer)
		vertexBuffer = other.vertexBuffer;
	vertexBuffer->AddRef();
	indexBuffer = other.indexBuffer;
	if (indexBuffer)
		indexBuffer->AddRef();
	indexCount = other.indexCount;
}

Mesh& Mesh::operator=(const Mesh& other)
{
	vertexBuffer = other.vertexBuffer;
	if (vertexBuffer)
		vertexBuffer->AddRef();
	indexBuffer = other.indexBuffer;
	if (indexBuffer)
		indexBuffer->AddRef();
	indexCount = other.indexCount;
	return *this;
}

Mesh& Mesh::operator=(Mesh&& other)
{
	vertexBuffer = other.vertexBuffer;
	other.vertexBuffer = nullptr;
	indexBuffer = other.indexBuffer;
	other.indexBuffer = nullptr;
	indexCount = other.indexCount;
	return *this;
}

Mesh::~Mesh()
{
	if (indexBuffer != nullptr)
	{
		indexBuffer->Release();
	}
	if (vertexBuffer != nullptr)
	{
		vertexBuffer->Release();
	}
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer* Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

void Mesh::Init(Vertex* vertices, int vCount, UINT* indices, int iCount, ID3D11Device * device)
{
	indexCount = iCount;

	// Define vertex description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	HRESULT vb = device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	// Create the INDEX BUFFER description ------------------------------------
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * iCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	HRESULT ib = device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
}
