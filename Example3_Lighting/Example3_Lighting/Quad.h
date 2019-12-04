// TriangleMesh.h
// Simple single triangle mesh (for example purposes).
// Mesh contains texture coordinates and normals.

#include "BaseMesh.h"

using namespace DirectX;

class Quad : public BaseMesh
{

public:
	Quad(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Quad();

	void sendData(ID3D11DeviceContext*);

protected:
	void initBuffers(ID3D11Device* device);

};
