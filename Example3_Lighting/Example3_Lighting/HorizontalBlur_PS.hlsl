Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenWidth;
	float3 padding;
};


struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
//	float weight0, weight1, weight2, weight3, weight4;
//	float4 colour;
//
//
//weight0 = 0.4062f; weight1 = 0.2442f; weight2 = 0.0545f;
//
//// Initialize the colour to black.
//colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
//
//float texelSize = 1.0f / screenWidth;
//
//colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize* -2.0f, 0.0f)) * weight2;
//colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize* -1.0f, 0.0f)) * weight1;
//colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
//colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize* 1.0f, 0.0f)) * weight1;
//colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize* 2.0f, 0.0f)) * weight2;
//
//colour.a = 1.0f;

float weight[11] = {0.000003,	0.000229,	0.005977,	0.060598,	0.24173,	0.382925,	0.24173,	0.060598,	0.005977,	0.000229,	0.000003};




	float4 textureColour = shaderTexture.SampleLevel(SampleType, input.tex, 0);

float texelSize = 1.0f / screenWidth / 0.5;

for (int i = -5; i < 6; i++)
{
	textureColour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * i, 0)) * weight[i + 5];
}
// Set the alpha channel to one.
 
textureColour.a = 1.0f;


return textureColour;
}
