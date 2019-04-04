#ifndef _LIGHTING_HLSL
#define _LIGHTING_HLSL

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2
#define MAX_LIGHT_COUNT         128
struct Light
{
	int		Type;
	float3	Direction;	// 16 bytes

	float	Range;
	float3	Position;	// 32 bytes

	float	SpotFalloff;
	float3	Color;		// 52 bytes


	float	AmbientIntensity;
	float	DiffuseIntensity;
	float	Padding_1;
	float	Padding_2;	//64 bytes
};


//------------Utility functions by Prof. Cascioli------

// Range-based attenuation function
float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.Position, worldPos);

	// Ranged-based attenuation
	float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));

	// Soft falloff
	return att * att;
}

///------------------------------------------------------------------

//Diffuse Light with Lambart
float Diffuse(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}

//Blinn-Phong Specular
float SpecularLight(float3 lightDirection, float3 normal, float3 dirToCamera, float shininess)
{
	float3 halfwayVector = normalize(lightDirection + dirToCamera);

	// Compare halflway vector and normal and raise to a power
	return shininess == 0 ? 0.0f : pow(max(dot(halfwayVector, normal), 0), shininess);
}

//Basic Lighiting

float3 BasicDirectLight(float3 normal, Light light, float3 cameraPos, float3 worldPos, float4 surfaceColor, float shininess, float roughness)
{

	float3 dirToCamera = normalize(cameraPos - worldPos);
	float3 dirToLight = normalize(-light.Direction);

	float diff = Diffuse(normal, dirToLight);
	float spec = SpecularLight(dirToLight, normal, dirToCamera, shininess); //Specular intensity needs to added in the Materials I guess.

	spec *= (1.0f - roughness);
	return (diff*surfaceColor.rgb + spec)*light.Color.rgb*light.DiffuseIntensity + light.Color.rgb*light.AmbientIntensity*surfaceColor.rgb;
}

float3 BasicPointLight(float3 normal, Light light, float3 cameraPos, float3 worldPos, float4 surfaceColor, float shininess, float roughness)
{

	float3 dirToCamera = normalize(cameraPos - worldPos);
	float3 dirToLight = normalize(light.Position - worldPos);

	float attn = Attenuate(light, worldPos);

	float diff = Diffuse(normal, dirToLight);
	float spec = SpecularLight(dirToLight, normal, dirToCamera, shininess);
	spec *= (1.0f - roughness);
	float3 ambientLight = light.Color.rgb*light.AmbientIntensity*attn*surfaceColor;
	float3 diffuseLight = (diff * surfaceColor.rgb + spec)* light.Color.rgb * light.DiffuseIntensity* attn;
	return (diffuseLight.rgb + ambientLight.rgb);
}


float3 BasicSpotLight(float3 normal, Light light, float3 cameraPos, float3 worldPos, float4 surfaceColor, float shininess, float roughness)
{
	float3 toLight = normalize(light.Position - worldPos);
	float penumbra = pow(saturate(dot(-toLight, light.Direction)), light.SpotFalloff);

	return BasicPointLight(normal, light, cameraPos, worldPos, surfaceColor, shininess, roughness) * penumbra;
}


#endif