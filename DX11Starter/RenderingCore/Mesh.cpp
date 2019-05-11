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
	const char* myfilepath = filePath.c_str();

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts

	// Open Assimp to load the file. Create object.
	Assimp::Importer importer;

	//Triangulate- convert corner points to vertices, ConvertToLeftHanded- convert to dx format (default OpenGL format), FlipUVs-for dx format 
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_FlipWindingOrder |
		aiProcess_FlipUVs |
		aiProcess_MakeLeftHanded);
	//1 mesh for now
	aiMesh* mesh = pScene->mMeshes[0];
	//store the positions, normals, uvs and push it in struct buffer 'verts'. Track 'vertCounter'.
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
	}
	//Calculate the indices
	for (UINT c = 0; c < mesh->mNumFaces; c++)
		for (UINT e = 0; e < mesh->mFaces[c].mNumIndices; e++)
		{
			indices.push_back(mesh->mFaces[c].mIndices[e]);
		}
	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the address of the first vert
	//
	// - The vector "indices" is similar. It's a vector of unsigned ints ands
	//    can be used directly for the index buffer: &indices[0] is the address of the first int
	//
	// - "vertCounter" is BOTH the number of vertices and the number of indices
	// - Yes, the indices are a bit redundant here (one per vertex).  Could you skip using
	//    an index buffer in this case?  Sure!  Though, if your mesh class assumes you have
	//    one, you'll need to write some extra code to handle cases when you don't.

	Init(&verts[0], (int)verts.size(), &indices[0], (int)indices.size(), device);
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

	// Calculate the tangents before copying to buffer
	CalculateTangents(vertices, vCount, indices, iCount);

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

// Calculates the tangents of the vertices in a mesh
// Code adapted from: http://www.terathon.com/code/tangent.html
void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numVerts;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthogonalize
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}