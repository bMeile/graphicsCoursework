//pixel shader for the marbled sphere
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float specPower;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
	float intensity = saturate(dot(normal, lightDirection)); //dot product of normal and light direction (returns value -1 to1)
	float4 colour = saturate(ldiffuse * intensity); //combine light intensity with diffuse colour
	colour += ambient; //adding ambient values to the diffuse colour
	return saturate(colour);
}

float4 calculateSpecular(float3 direction, float3 normal, float3 viewVector, float4 specularColour, float4 specularPower)
{
	//blinn-phong lighting calculations
	float3 halfVEctor = normalize(direction + viewVector);
	float4 SpecularIntensity = pow(max(dot(normal, halfVEctor), 0.001), specularPower);
	return saturate(specularColour * SpecularIntensity);

}
float4 main(InputType input) : SV_TARGET
{
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	//diffuse and ambient lightings
	float4 lightColour = calculateLighting(-direction, input.normal, diffuse);

	//spacular ligting calculations
	float4 specularLighting = calculateSpecular(-direction, input.normal, input.viewVector, specular, specPower);

	//Adding specular lighting to ambien and deffuse
	lightColour += specularLighting;

	//returning lighting and texture
	return saturate(textureColour* lightColour);


}