Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4  diffuse;
	float4 ambient;
	float4 specColour;
	float3 direction;
	float specPower;
	float4 lightPos;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
	float3 viewVector : TEXCOORD3;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
	float intensity = saturate(dot(normal, -lightDirection)); //dot product of normal and light direction (returns value -1 to1)
	float4 colour = saturate(ldiffuse * intensity); //combine light intensity with diffuse colour
	colour += ambient; //adding ambient values to the diffuse colour
	return saturate(colour);
}

//approximates the relative surface area of microfacets exactly aligned to the halfway vector
float normalDistribution(float3 normal, float3 halfway, float m)//float3 N, float3 H, float m)
{
	float rougness2 = m * m;
	float h = max(dot(normal, halfway), 0.001);
	float x = h * h;
	float longNon = (x - 1.0) / x;
	float d = 3.14 * rougness2 * x * x;
	float D = exp(longNon / rougness2) / d;
	return D;
}

float schlickGGX(float NdotV, float k)
{
	float down = NdotV * (1.0 - k) + k;
	return NdotV / down;
}

float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = schlickGGX(NdotV, k);
	float ggx2 = schlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

float3 fresnelS(float cosTheta, float3 F0) //cosTheta = dot(n, viewDitrection)
{
	return F0 + (1.0 - F0) * pow(1 - cosTheta, 5.0);
}

float4 calculateSpecular(float3 direction, float3 normal, float3 viewVector, float4 specularColour, float4 specularPower)
{
	//cook torrance
	float3 halfway = normalize(direction + viewVector);

	float k = ((specularPower + 1)*(specularPower + 1)) / 8;

	float3 F0 = float3(0.98, 0.82, 0.76); //copper
	float cosTheta = dot(normal, direction);

	float D = normalDistribution(normal, halfway, specularPower);
	float F = GeometrySmith(normal, viewVector, direction, k);
	float G = fresnelS(cosTheta, F0);

	float kSpec = (D * F * G) / (4 * (dot(viewVector, normal)) * (dot(normal, direction)));

	float dif = diffuse / 3.14;

	return dif + kSpec;

}

float4 main(InputType input) : SV_TARGET
{
	float linearF = 0.125f;
	float constant = 1.0f;
	float quadF = 0.001f;

	float dist = distance(lightPos, input.worldPosition);


	float attenuation = 1 / (constant + (linearF * dist) + (quadF * pow(dist, 2)));

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);
float3 lightvector = normalize(lightPos.xyz - input.worldPosition);
	//diffuse and ambient lightings
	float4 lightColour = calculateLighting(lightvector, input.normal, diffuse * attenuation);

	
	//spacular ligting calculations
	float4 specularLighting = calculateSpecular(lightvector, input.normal, input.viewVector, specColour, 0.1f);

	//Adding specular lighting to ambien and deffuse
	lightColour += (specularLighting+ambient);

	//return float4(input.normal, 1.0f);

	//returning lighting and texture
	return saturate(lightColour) * textureColour;

}