#pragma once

#include "DXF.h"
#include <stdio.h>

using namespace std;
using namespace DirectX;

class marbleShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT3 direction;
		float specPower;
	};

	struct CameraBufferType
	{
		XMFLOAT3 camPosition;
		float padding2;
	};

public:
	marbleShader(ID3D11Device* device, HWND hwnd);
	~marbleShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, XMFLOAT3 cameraPosition);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* lightBuffer;

};


