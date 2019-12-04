// Tessellation mesh.
// Builds a simple triangle mesh for tessellation demonstration
// Overrides sendData() function for different primitive topology
#include "Quad.h"

// initialise buffers and load texture.
Quad::Quad(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}

// Release resources.
Quad::~Quad()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build triangle (with texture coordinates and normals).
void Quad::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	int fn = 100;
	int num = fn * fn * 4;
	vertexCount = num;
	indexCount = num;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];
	float u = 0.0f;
	float v = 0.0f;
	  
	float uvNum = 1.0f ;

	// Load the vertex array with data.
	int a = 0;
	for (int i = 0; i < fn; i++)
	{
		for (int z = 0; z < fn; z++)
		{
			vertices[a].position = XMFLOAT3(i, 0.0f, z);  // Top.
			vertices[a].texture = XMFLOAT2(u, v);
			vertices[a].normal = XMFLOAT3(0.0f, 1.0, 0.0f);
			a++;
			vertices[a].position = XMFLOAT3(i, 0.0f, z + 1);  // Bottom left.
			vertices[a].texture = XMFLOAT2(u, v + uvNum);
			vertices[a].normal = XMFLOAT3(0.0f, 1.0, 0.0f);
			a++;
			vertices[a].position = XMFLOAT3(i + 1.0f, 0.0f, z + 1.0f);  // Bottom right.
			vertices[a].texture = XMFLOAT2(u + uvNum, v + uvNum);
			vertices[a].normal = XMFLOAT3(0.0f, 1.0, 0.0f);
			a++;
			vertices[a].position = XMFLOAT3(i+1 , 0.0f, z);  // Top right.
			vertices[a].texture = XMFLOAT2(u + uvNum, v);
			vertices[a].normal = XMFLOAT3(0.0f, 1.0, 0.0f);
			a++;
			u += uvNum;
		}
		
		v +=v;
	}


	// Load the index array with data.
	for (int i = 0; i < num; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

// Override sendData() to change topology type. Control point patch list is required for tessellation.
void Quad::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}

