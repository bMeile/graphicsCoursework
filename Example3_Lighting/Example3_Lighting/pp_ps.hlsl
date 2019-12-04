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



	float4 textureColour = shaderTexture.SampleLevel(SampleType, input.tex, 0);

	float brightness = textureColour.x + textureColour.y + textureColour.z;

	if (brightness < 0.75f)
	{
		textureColour = 0;
	} 
	else
	{
		textureColour *= 1.0;
	}


	//float weight[11] = {0.000003,	0.000229,	0.005977,	0.060598,	0.24173,	0.382925,	0.24173,	0.060598,	0.005977,	0.000229,	0.000003};

	////float weight[9] = {0.000, 0.00, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0};


	//float4 colour;

	//// Initialize the colour to black.
	//colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//float texelSize = 1.0f / screenWidth;

	//// Add the horizontal pixels to the colour by the specific weight of each.

	////vector3 acc = vec3(0);

	////const float delta = 2.0 / float(samples);

	////for (float i = -1.0; i <= 1.0; i += delta)
	////{
	////	acc += texture(tex, input.tex - vec2(angle.x * i, angle.y * i));
	////}


	////return vec4(delta * acc, 0);

	//for (int i = -5; i < 6; i++)
	//{
	//	textureColour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * i, 0)) * weight[i + 5];

	//}

	//// Set the alpha channel to one.
	//textureColour.a = 1.0f;

//	textureColour+= shaderTexture.SampleLevel(SampleType, input.tex, 0);

	return textureColour;
}
