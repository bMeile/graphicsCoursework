// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tesQuad_vs.cso", L"tesQuad_hs.cso", L"tesQuad_ds.cso", L"tesQuad_ps.cso");
}


TessellationShader::~TessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (timeBuffer)
	{
		layout->Release();
		timeBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader( WCHAR* vsFilename, WCHAR* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	D3D11_SAMPLER_DESC samplerDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC tesFBufferDesc;
	tesFBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesFBufferDesc.ByteWidth = sizeof(tesFBufferType);
	tesFBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesFBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesFBufferDesc.MiscFlags = 0;
	tesFBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tesFBufferDesc, NULL, &tesFBuffer);

	D3D11_BUFFER_DESC timeBufferDesc;
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(timeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

}

void TessellationShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix,  float totalTime, XMFLOAT4 campos, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, Light* light, Light* BPlight, XMFLOAT3 cameraPosition)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	tesFBufferType* tesFPtr;
	deviceContext->Map(tesFBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesFPtr = (tesFBufferType*)mappedResource.pData;
	tesFPtr->factor1 = 0;
	tesFPtr->factor2 = 0;
	tesFPtr->factor3 = 0;
	tesFPtr->middle = 0;
	tesFPtr->camPos = cameraPosition;
	tesFPtr->padding = 0.0f;
	deviceContext->Unmap(tesFBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesFBuffer);


	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse[0] = light->getDiffuseColour();
	lightPtr->diffuse[1] = BPlight->getDiffuseColour();

	lightPtr->ambient[0] = light->getAmbientColour();
	lightPtr->ambient[1] = BPlight->getAmbientColour();

	lightPtr->specColour[0] = light->getSpecularColour();
	lightPtr->specColour[1] = BPlight->getSpecularColour();

	lightPtr->direction = BPlight->getDirection();

	lightPtr->specularPow = light->getSpecularPower();
//	lightPtr->specularPow[1] = BPlight->getSpecularPower();

	lightPtr->lightPos = light->getPosition();
	lightPtr->specPower2 = BPlight->getSpecularPower();

	lightPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	lightPtr->time = totalTime;

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);
	// Send light data to pixel shader

	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPos = cameraPosition;
	cameraPtr->padding3 = 0.0f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->DSSetConstantBuffers(2, 1, &cameraBuffer);
	deviceContext->HSSetConstantBuffers(1, 1, &cameraBuffer);

	timeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (timeBufferType*)mappedResource.pData;
	timePtr->timeT = totalTime;
	timePtr->height = 0.3;
	timePtr->frequency = 1;
	timePtr->speed = 0.8;

	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->DSSetShaderResources(0, 1, &texture2);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
}


