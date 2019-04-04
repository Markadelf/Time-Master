#include "lighting.hlsli"
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};



// External Data for 
cbuffer externalData : register(b0)
{
	Light lights[MAX_LIGHT_COUNT];
	int lightCount;
	float3 cameraPos;
	float  shinniness;
};

Texture2D diffuseTexture : register(t0);
Texture2D roughnessTexture : register(t1);
SamplerState basicSampler : register(s0);



// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float4 surfaceColor = pow(diffuseTexture.Sample(basicSampler, input.uv), 2.2);
	float roughness = roughnessTexture.Sample(basicSampler, input.uv).r;
	roughness = lerp(0, roughness, 1);// x*(1-s) + y*s lerp(x,y,s)
	
	float3 finalColor;
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			finalColor+= BasicDirectLight(input.normal, lights[i], cameraPos, input.worldPos, surfaceColor, shinniness, roughness);
			break;
		case LIGHT_TYPE_POINT:
			finalColor += BasicPointLight(input.normal, lights[i], cameraPos, input.worldPos, surfaceColor, shinniness, roughness);
			break;
		case LIGHT_TYPE_SPOT:
			finalColor += BasicSpotLight(input.normal, lights[i], cameraPos, input.worldPos, surfaceColor, shinniness, roughness);
			break;
		}
	}
	return float4(finalColor, 0);

}