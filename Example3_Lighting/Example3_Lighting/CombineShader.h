#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class CombineShader : public BaseShader
{
public:
	CombineShader(ID3D11Device* device, HWND hwnd);
	~CombineShader();
	void setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, ID3D11ShaderResourceView * texture1, ID3D11ShaderResourceView * texture2);

	
private:
	void initShader(WCHAR*, WCHAR*);

	
private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
};

