Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenHeight;
	float3 padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
		//float weight0, weight1, weight2;
		//float4 colour;
		//// Create the weights that each neighbour pixel will contribute to the blur.
		//	weight0 = 0.4062f; weight1 = 0.2442f; weight2 = 0.0545f;
		//	// Initialize the colour to black.
		//	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
		//	float texelSize = 1.0f / screenHeight;
		//	// Add the vertical pixels to the colour by the specific weight of each.
		//	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize* -2.0f)) * weight2;
		//	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize* -1.0f)) * weight1;
		//	colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
		//	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize* 1.0f)) * weight1;
		//	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize* 2.0f)) * weight2;
		//	// Set the alpha channel to one.
		//	colour.a = 1.0f;
		//	return colour;


	float weight[11] = {0.000003,	0.000229,	0.005977,	0.060598,	0.24173,	0.382925,	0.24173,	0.060598,	0.005977,	0.000229,	0.000003};


	float4 textureColour = shaderTexture.SampleLevel(SampleType, input.tex, 0);


			float texelSize = 1.0f / screenHeight / 0.5;

			for (int i = -5; i < 6; i++)
			{
				textureColour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * i)) * weight[i + 5];

			}
			// Set the alpha channel to one.
			textureColour.a = 1.0f;


			return textureColour;
}

