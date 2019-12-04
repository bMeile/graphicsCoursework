// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "hmShader.h"
#include "marbleShader.h"
#include "PostProcessingShader.h"
#include "VerticalBlur.h"
#include "HorizontalBlur.h"
#include "TextureShader.h"
#include "Quad.h"
#include "TessellationShader.h"
#include "CombineShader.h"
#include <cmath>


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();
	void firstPass();
	void downSamplePass();
	void horizontalBlur();
	void verticalBlur();
	void postProcessing();
	void upSamplePass();
	void finalPass();


protected:
	bool render();
	void gui();

private:
	float factorOne, factorTwo, factorThree, midFactor;
	hmShader* shader;
	marbleShader* marbleShade;
	PlaneMesh* mesh;
	PlaneMesh* lava;
	PlaneMesh* waterPlane;
	Light* light;
	Light* BPlight;
	SphereMesh* marbleMesh;

	OrthoMesh* orthoMesh;
	OrthoMesh* orthoMeshSmall;

	RenderTexture* renderTexture;
	RenderTexture* ppTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* downSampleBlurTexture;
	RenderTexture* finalRenderTexture;
	PostProcessingShader* postProcessingShader;
	HorizontalBlur* horBlur;
	VerticalBlur* verBlur;
	TextureShader* textureShader;
	TessellationShader* tesShader;
	CombineShader* combShader;
	Quad* quad;
	XMFLOAT4 camPos;
	Light* pointLight;

	float height;
	float totalTime;
	float downSammpleValue = 0.8f;

	XMFLOAT3 light1posi = XMFLOAT3(50.0f, 20.0f, 50.0f);
	XMFLOAT3 lightDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int rtInt = 4;
};

#endif