// Tessellation Hull Shader
// Prepares control points for tessellation

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer tesFBuffer : register(b0)
{
	float factor1;
	float factor2;
	float factor3;
	float middle;
	float3 camPos;
	float padding;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

};

cbuffer CameraBuffer : register(b1)
{
	float3 cameraPosition;
	float padding3;
};


struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;

};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

};
float getTessenationAmount(float3 pos);
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float3 cPosition = camPos.xyz;

	float3 midPoint0 = (inputPatch[0].position + inputPatch[1].position) / 2;
	float3 midPoint1 = (inputPatch[0].position + inputPatch[2].position) / 2;
	float3 midPoint2 = (inputPatch[2].position + inputPatch[3].position) / 2;
	float3 midPoint3 = (inputPatch[1].position + inputPatch[3].position) / 2;

	float dis0 = distance(cPosition, midPoint0);
	float dis1 = distance(cPosition, midPoint2);


	//float max1 = max(dis0, dis1);


	int m;


	for (int i = 0; i < 32; i++)
	{
		if (dis0 > i)
		{
			m = 32 - i;
			if (m >= 5)
			{
				output.edges[0] = m;
				output.edges[2] = m - 1;
				output.edges[1] = m - 1;
				output.edges[3] = m;

				output.inside[0] = m;
				output.inside[1] = m;
			}
			else if (m < 5)
			{
				m = 4;

				output.edges[0] = m;
				output.edges[2] = m;
				output.edges[1] = m;
				output.edges[3] = m;

				output.inside[0] = m;
				output.inside[1] = m;

			}

		}
	}




	return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;

	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;
	output.tex = patch[pointId].tex;
	output.normal = patch[pointId].normal;

	return output;
}
