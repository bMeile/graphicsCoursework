// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

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

cbuffer timeBuffer : register(b1)
{
	float timeT;
	float height;
	float frequency;
	float speed;
};

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
	float padding3;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
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
	float3 viewVector : TEXCOORD3;


};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{

	float3 vertexPosition;
	OutputType output;
	float4 textureColour;
	float2 textureCoord;
	float3 normals;

	// Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).


	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.x);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.x);
	vertexPosition = lerp(v1, v2, uvwCoord.y);

	//////set the texture coordinates based on tessellation
	float2 v3 = lerp(patch[0].tex, patch[1].tex, uvwCoord.x);
	float2 v4 = lerp(patch[3].tex, patch[2].tex, uvwCoord.x);
	output.tex = lerp(v3, v4, uvwCoord.y);

	////set the texture coordinates based on tessellation
	float3 v5 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 v6 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	float3 normalThing= lerp(v5, v6, uvwCoord.x);

	//for height map
	textureColour = texture0.SampleLevel(sampler0, output.tex, 0);//calcuates how much to manipulate the vertex position
	vertexPosition.y += textureColour * 3.75; //changes the position based on the colour



//
//	//https://medium.com/@joshmarinacci/water-ripples-with-vertex-shaders-6a9ecbdf091f
	////vertexPosition.y += height * (sin(vertexPosition.x * frequency + timeT * speed));		// +(cos(vertexPosition.y * 30 + time * 2) / 50);
	float dx = vertexPosition.x -50;
	float dz = vertexPosition.z -50;
	float freq = sqrt(pow(dx, 2) + pow(dz, 2));
	float amp = 1.85;
	float angle = -timeT * 1.5 + freq * 1.0;


	////do the calculus thing explained
	//sin(-timeT * 1.5 + sqrt(pow((vertexPosition.x - 50), 2) + pow((vertexPosition.z - 50), 2)) * 1.0)*0.85f

	vertexPosition.y += sin(angle)*amp ;



	//float vector1Middle =(( amp * cos(sqrt((dx*dx) - (100*dx)+(dz*dz)+5000-(100*dz))) + freq) * (dx-50)) / (sqrt(pow(dx, 2)) - (100*dx) + (pow(dz, 2)) + 5000-(100*dz));
	//float vector2Middle = dz * cos(dx*dz + timeT) * dx;
	//float vector2Middle = ((amp * cos(sqrt((dx*dx) - (100 * dx) + (dz*dz) + 5000 - (100 * dz))) + freq) * (dz - 50)) / (sqrt(pow(dz, 2)) - (100 * dz) + (pow(dx, 2)) + 5000 - (100 * dx));


	//float first = vector1Middle;
	float secound = 0.0;
//	float third = ( vector2Middle);


	float3 objectNormal = normalize(float3(0.0, (dx-50)*cos(dx*dz) * (dz-50), 1.0));
	float3 objectNormal2 = normalize(float3(1.0, (dz-50)* cos(dx*dz) * (dx-50), 0.0));
	//

	//float3 objectNormal = normalize(float3(first, secound, third));

	//float3 objectNormal = normalize(float3(0.0, -amp * freq * cos(angle), 1.0));
	//output.normal = normalThing * objectNormal;

	//float3 objectNormal = normalize(float3(0.0, amp * freq * (cos(angle)*0.125), 1.0));
	////float3 objectNormal2 = normalize(float3(1.0, amp * freq * (cos(angle)*1.125), 0.0));

	//float3 objectNormal = normalize(float3(0.0, ((amp * cos(sqrt((dx*dx) - (100 * dx) + (dz*dz) + 5000 - (100 * dz))) + freq) * (dx - 50)) / (sqrt(pow(dx, 2)) - (100 * dx) + (pow(dz, 2)) + 5000 - (100 * dz)) , 1.0));
	//float3 objectNormal2 = normalize(float3(1.0, ((amp * cos(sqrt((dx*dx) - (100 * dx) + (dz*dz) + 5000 - (100 * dz))) + freq) * (dz - 50)) / (sqrt(pow(dz, 2)) - (100 * dz) + (pow(dx, 2)) + 5000 - (100 * dx)), 0.0));


	output.normal = objectNormal * normalThing * objectNormal2;





	////// Calculate the position of the vertex as viewed by the light source.
	output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//// Calculate the camera view vector
	float4 worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.worldPosition = worldPosition;
	output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);

	return output;
}

