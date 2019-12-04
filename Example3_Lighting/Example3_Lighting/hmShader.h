#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class hmShader : public BaseShader
{
private:
	
	struct heightBufferType
	{
		float height;
		XMFLOAT3 padding;
		
	};
struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specColour;
		XMFLOAT3 direction;
		float specularPow;
		XMFLOAT4 lightPos;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float padding3;
	};


public:
	hmShader(ID3D11Device* device, HWND hwnd);
	~hmShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, float height, Light* light, XMFLOAT3 cameraPosition);


private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* heightBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
};

