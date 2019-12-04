// vertex shader for height mapped plane

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer heightBuffer : register(b1)
{	
	float height;
	float3 padding;
	
};

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
	float padding3;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
	float3 viewVector : TEXCOORD3; //camera view
};

OutputType main(InputType input)
{
	OutputType output;

	//get texture colour for decplacement mapping
	float4 textureColour = texture0.SampleLevel(sampler0, input.tex, 0);//sampler state, location and mipMap level

	//offset position based on the texture colour
	input.position.x *= 1.5;
	input.position.y += textureColour * height * 1.75;
	input.position.z *= 1.5;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Store worldPosition for pixel shader
	output.worldPosition = mul(input.position, worldMatrix).xyz;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex as viewed by the light source.
	output.lightViewPos = mul(input.position, worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

	// Calculate the camera view vector.
	float4 worldPosition = mul(input.position, worldMatrix);
	output.worldPosition = worldPosition;
	output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);

	return output;
}