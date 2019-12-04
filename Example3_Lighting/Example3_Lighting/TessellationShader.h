// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
private:
	struct tesFBufferType
	{
		float factor1;
		float factor2;
		float factor3;
		float middle;

		XMFLOAT3 camPos;
		float padding;
	};

	struct timeBufferType
	{
		float timeT;
		float height;
		float frequency;
		float speed;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuse[2];
		XMFLOAT4 ambient[2];
		XMFLOAT4 specColour[2];
		XMFLOAT3 direction;
		float specularPow;
		XMFLOAT3 lightPos;
		float specPower2;
		XMFLOAT3 padding;
		float time;
	};


	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float padding3;
	};

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, float totalTime, XMFLOAT4 campos, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * texture2, Light * light, Light * BPlight, XMFLOAT3 cameraPosition);


private:
	void initShader(WCHAR* vsFilename,  WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tesFBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
};