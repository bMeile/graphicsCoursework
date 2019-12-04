// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"


App1::App1()
{
	mesh = nullptr;
	shader = nullptr;

	height = 20.0f;
	totalTime = 0.0f;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	//textures
	textureMgr->loadTexture("displacementMap", L"../res/Volcano.gif");
	textureMgr->loadTexture("displacementMapLava", L"../res/la.gif");
	textureMgr->loadTexture("lavaDM", L"../res/lavaDM.jpg");
	textureMgr->loadTexture("marble", L"../res/marble.jpg");
	textureMgr->loadTexture("vol", L"../res/vol.jpg");
	textureMgr->loadTexture("water", L"../res/water.jpg");
	textureMgr->loadTexture("waterDark", L"../res/waterDark.png");
	textureMgr->loadTexture("fox", L"../res/fox.png");
	textureMgr->loadTexture("waterhm", L"../res/index.png");
	textureMgr->loadTexture("lava", L"../res/lava.png");

	factorOne = 2;
	factorTwo = 5;
	factorThree = 3;
	midFactor = 10;

	// Create Mesh objects
	marbleMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext()); //marble sphere mesh
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext()); //heightmapped plane mesh
	waterPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext()); //heightmapped plane mesh
	quad = new Quad(renderer->getDevice(), renderer->getDeviceContext());
	lava = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext()); //heightmapped plane mesh

	//Create Shader objects
	shader = new hmShader(renderer->getDevice(), hwnd); //for height mapped plane
	marbleShade = new marbleShader(renderer->getDevice(), hwnd); //for marble sphere
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	tesShader = new TessellationShader(renderer->getDevice(), hwnd);
	horBlur = new HorizontalBlur(renderer->getDevice(), hwnd);
	verBlur = new VerticalBlur(renderer->getDevice(), hwnd);
	combShader = new CombineShader(renderer->getDevice(), hwnd);



	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	orthoMeshSmall = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth * downSammpleValue, screenHeight * downSammpleValue, 0, 0);
	downSampleBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth * downSammpleValue, screenHeight * downSammpleValue, SCREEN_NEAR, SCREEN_DEPTH);
	postProcessingShader = new PostProcessingShader(renderer->getDevice(), hwnd);
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	ppTexture = new RenderTexture(renderer->getDevice(), screenWidth , screenHeight , SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth*downSammpleValue, screenHeight*downSammpleValue, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth*downSammpleValue, screenHeight*downSammpleValue, SCREEN_NEAR, SCREEN_DEPTH);
	finalRenderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);



	// Volcano
	light = new Light;
	light->setDiffuseColour(1.0f, 0.6f, 0.0f, 1.0f);
	light->setAmbientColour(0.9f, 0.6f, 0.6f, 1.0f);
	light->setSpecularColour(1.0f, 0.4f, 0.0f, 1.0f);
	light->setDirection(0.0f,-1.0, 0.0f);
	light->setSpecularPower(0.09f);
	light->setPosition(0, 10, 0.0);

	// Water
	pointLight = new Light;
	pointLight->setDiffuseColour(1.0f, 0.05f, 0.0f, 0.0f);
	pointLight->setAmbientColour(0.02f, 0.01f, 0.02f, 0.0f);
	pointLight->setSpecularColour(2.0f, 0.25f, 0.05f, 0.0f);
	pointLight->setDirection(0.0f, -0.01, 1.0f);
	pointLight->setSpecularPower(0.75f);
	pointLight->setPosition(light1posi.x, light1posi.y, light1posi.z);

	// Blinn-Phong lighting
	BPlight = new Light;
	BPlight->setAmbientColour(0.002f, 0.002f, 0.002f, 0.0f);
	BPlight->setDiffuseColour(0.0f, 0.0f, 0.0f, 0.0f);
	lightDir = XMFLOAT3(lightDir.x, -0.05f, -1.0f);
	BPlight->setDirection(lightDir.x, lightDir.y, lightDir.z);
	BPlight->setSpecularPower(0.999999f);
	BPlight->setSpecularColour(0.3f, 0.3f, 0.3f, 0.0f);

	camera->setPosition(0.0f, 15, -40.0f);
	camera->setRotation(0.0f, 20.0f, 0.0f);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (mesh) { delete mesh; mesh = 0; }
	if (shader) { delete shader; shader = 0; }
	

}


bool App1::frame()
{
	bool result;
	// update lights
	pointLight->setPosition(light1posi.x, light1posi.y, light1posi.z);
	BPlight->setDirection(lightDir.x, lightDir.y, lightDir.z);



	result = BaseApplication::frame();

	if (!result)
	{
		return false;
	}
	totalTime += timer->getTime();
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{

	firstPass();

	postProcessing();

	downSamplePass();

	horizontalBlur();

	verticalBlur();

	upSamplePass();

	finalPass();

	// lerp(textureMgr->loadTexture("waterDark"), textureMgr->loadTexture("waterDark"), 0.5);

	return true;
}

void App1::firstPass()
{
	// Set the render target to be the render to texture and clear it
	renderTexture->setRenderTarget(renderer->getDeviceContext());
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.2f, 1.0f);

	XMMATRIX move, finalM, worldMatrix, viewMatrix, projectionMatrix, YworldMatrix;

	//Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 0.0f, 0.2f, 1.0f);

	//Generate the view matrix based on the camera's position.
	camera->update();


	////Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//atrices for water plane
	YworldMatrix = XMMatrixScaling(10.0, 1.0, 5.0); //10/5

	move = XMMatrixTranslation(-50.0, 2.0, -50.0);
	finalM = XMMatrixMultiply(YworldMatrix, move);

	XMMATRIX translateVolcano = XMMatrixTranslation(0.0, -5.0, 0.0);
	XMMATRIX translateVolcano2 = XMMatrixTranslation(0.0, -2.0, 0.0);

	//Send geometry data, set shader parameters, render object with shader
	mesh->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), translateVolcano, viewMatrix, projectionMatrix, textureMgr->getTexture("vol"), textureMgr->getTexture("displacementMap"), 30.0f, light, camera->getPosition());
	shader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	XMMATRIX matRotation = XMMatrixRotationY(20);
	XMMATRIX translateLava= XMMatrixTranslation(0.0, 0.0, 50.0);
	XMMATRIX moveLava = matRotation * translateLava;

	//Send geometry data, set shader parameters, render object with shader
	lava->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), translateVolcano2, viewMatrix, projectionMatrix, textureMgr->getTexture("lava"), textureMgr->getTexture("displacementMapLava"), 30.0f, light, camera->getPosition());
	shader->render(renderer->getDeviceContext(), lava->getIndexCount());

	XMMATRIX rotate;
	rotate = XMMatrixRotationY(1.57);

	camPos = XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 0.0);

	quad->sendData(renderer->getDeviceContext());
	tesShader->setShaderParameters(renderer->getDeviceContext(),finalM, viewMatrix, projectionMatrix, totalTime, camPos, textureMgr->getTexture("water"), textureMgr->getTexture("waterhm"), pointLight, BPlight, camera->getPosition());
	tesShader->render(renderer->getDeviceContext(), quad->getIndexCount());

	//Send geometry data, set shader parameters, render object with shader
	marbleMesh->sendData(renderer->getDeviceContext());
	marbleShade->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("marble"), BPlight, camera->getPosition());
	marbleShade->render(renderer->getDeviceContext(), marbleMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();

	//// Render GUI
	//gui();



	//// Present the rendered scene to the screen.
	//renderer->endScene();

}

void App1::postProcessing()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)ppTexture->getTextureWidth();
	ppTexture->setRenderTarget(renderer->getDeviceContext());
	ppTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = ppTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	postProcessingShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderTexture->getShaderResourceView(), screenSizeX);
	postProcessingShader->render(renderer->getDeviceContext(), orthoMeshSmall->getIndexCount());


	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::downSamplePass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)downSampleBlurTexture->getTextureWidth();
	downSampleBlurTexture->setRenderTarget(renderer->getDeviceContext());
	downSampleBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = downSampleBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);

	orthoMeshSmall->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, ppTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMeshSmall->getIndexCount());

	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::horizontalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);
	//HoriBlurTexture->setRenderTarget(renderer->getDeviceContext());
	//downSampleBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMeshSmall->sendData(renderer->getDeviceContext());
	horBlur->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, downSampleBlurTexture->getShaderResourceView(), screenSizeX);
	horBlur->render(renderer->getDeviceContext(), orthoMeshSmall->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::verticalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	//downSampleBlurTexture->setRenderTarget(renderer->getDeviceContext());
	//downSampleBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	orthoMeshSmall->sendData(renderer->getDeviceContext());
	verBlur->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY);
	verBlur->render(renderer->getDeviceContext(), orthoMeshSmall->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::upSamplePass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)renderTexture->getTextureWidth();
	finalRenderTexture->setRenderTarget(renderer->getDeviceContext());
	finalRenderTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = finalRenderTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);



	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());

	if (rtInt == 0) {
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	else if (rtInt == 1) {
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, ppTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	else if (rtInt == 2) {
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downSampleBlurTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	else if (rtInt == 3) {
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	else if (rtInt == 4) {
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, finalRenderTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	else if (rtInt == 5) {
		combShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, finalRenderTexture->getShaderResourceView(), renderTexture->getShaderResourceView());
		combShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	renderer->setZBuffer(true);

	// Render GUI
	gui();



	// Present the rendered scene to the screen.
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::SliderFloat3("Light Position", (float*)&light1posi, -100.0f, 100.0f);
	ImGui::SliderFloat3("Light Direction", (float*)&lightDir, -1.0f, 1.0f);
	ImGui::SliderInt("RenderTexture", (int*)&rtInt, 0, 5);

    // Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
