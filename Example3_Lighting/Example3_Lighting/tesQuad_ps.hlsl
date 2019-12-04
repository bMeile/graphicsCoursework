
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4  diffuse[2];
	float4 ambient[2];
	float4 specColour[2];
	float3 direction;
	float specPower;
	float3 lightPos;
	float specPower2;
	float3 padding;
	float time;
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
float4 calculateDirectionalLighting(float3 ldirectionvector, float3 normal, float4 ldiffuse)
{
	float intensity = saturate(dot(normal, ldirectionvector)); //dot product of normal and light direction (returns value -1 to1)
	float4 colour = saturate(ldiffuse * intensity); //combine light intensity with diffuse colour
//	colour += ambient[1]; //adding ambient values to the diffuse colour
	return colour;
}


float4 calculateSpecularBP(float3 direction, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	//blinn-phong lighting calculations
	float3 halfVector = normalize(direction + viewVector);
	float SpecularIntensity = pow(max(dot(normal, halfVector), 0.00001), specularPower);
	return saturate(specularColour * SpecularIntensity);

}


// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculatePointLighting(float3 lpostionvector, float3 normal, float4 ldiffuse)
{
	float intensity = saturate(dot(normal, lpostionvector)); //dot product of normal and light direction (returns value -1 to1)
	float4 colour = saturate(ldiffuse * intensity); //combine light intensity with diffuse colour
	return colour;
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

float4 calculateSpecular(float3 direction, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	//cook torrance
	float3 halfway = normalize(direction + viewVector);

	float k = ((specularPower + 1)*(specularPower + 1)) / 8;

	float3 F0 = float3(0.2, 0.2, 0.2); //copper
	float cosTheta = dot(normal, direction);

	float D = normalDistribution(normal, halfway, specularPower);
	float F = GeometrySmith(normal, viewVector, direction, k);
	float G = fresnelS(cosTheta, F0);

	float kSpec = (D * F * G) / (4 * (dot(viewVector, normal)) * (dot(normal, direction)));

	float dif = diffuse[0] / 3.14;

	return specularColour * kSpec;
}



float4 main(InputType input) : SV_TARGET
{
	float4 lightColour, pointLightColour, directionalLightColour;
	//float lightNr = 1.0f;

//moves the water texture
	input.tex += time / 10;
	// Sample the texture
	float4 textureColour = texture0.Sample(sampler0, input.tex);
	

	float3 lightvector = normalize(lightPos - input.worldPosition);

	//if (lightNr == 0.0f) { //padding 0 is point light
		float linearF = 0.000025f;
		float constant = 0.45f;
		float quadF = 0.0f;

		float dist = distance(lightPos, input.worldPosition);

		float attenuation = 1 / (constant + (linearF * dist) + (quadF * pow(dist, 2)));

		//spec1
		float4 specularPointLighting = calculateSpecular(lightvector, input.normal, input.viewVector, specColour[0], specPower);
		//diffuse1
		pointLightColour = calculatePointLighting(lightvector, input.normal, diffuse[0]) * attenuation;
		//Adding specular1 lighting to ambient1 and diffuse1
		pointLightColour += (specularPointLighting + ambient[0]);
lightColour = pointLightColour;
	/*}*/
	//else
	//{
	
	// directional light

	//spec2
	float4 specularLighting2 = calculateSpecularBP(-direction, input.normal, input.viewVector, specColour[1], specPower2);
	//diffuse2
	directionalLightColour = calculateDirectionalLighting(-direction, input.normal, diffuse[1])*attenuation;
	//adding specular2 and ambient2 to diffuse2
	directionalLightColour += (specularLighting2 + ambient[1]);

	/*}*/

	
	lightColour += directionalLightColour;

	//returning lighting and texture
	//return float4(input.normal, 1.0f);
	return saturate(lightColour) *textureColour;
}