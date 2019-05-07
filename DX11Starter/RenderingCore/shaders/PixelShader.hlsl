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
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
	float4 posForShadow : SHADOW;
};



// External Data for 
cbuffer externalData : register(b0)
{
	Light lights[MAX_LIGHT_COUNT];
	int lightCount;
	float3 cameraPos;
	float  shinniness;
    uint shadowRes;
    int shadowSmooth;
};

Texture2D diffuseTexture    : register(t0);
Texture2D roughnessTexture  : register(t1);
Texture2D normalTexture     : register(t2);
TextureCube Sky             : register(t3);
Texture2D ShadowMap         : register(t4);

SamplerState basicSampler               : register(s0);
SamplerComparisonState ShadowSampler    : register(s1);



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
	//return float4(input.tangent,0);
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Sample from the normal map (and UNPACK values)
	float3 normalFromMap = normalTexture.Sample(basicSampler, input.uv).rgb * 2 - 1;

	// Create the matrix that will allow us to go from tangent space to world space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	// Overwrite the initial normal with the version from the
	// normal map, after we've converted to world space
	input.normal = normalize(mul(normalFromMap, TBN));
	//return float4(input.normal, 0);
	float4 surfaceColor = pow(abs(diffuseTexture.Sample(basicSampler, input.uv)), 2.2);
	float roughness = roughnessTexture.Sample(basicSampler, input.uv).r;
	roughness = lerp(0, roughness, 1);// x*(1-s) + y*s lerp(x,y,s)

	// Shadow map calculations
	float2 shadowUV = input.posForShadow.xy / input.posForShadow.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;

	// Actual depth of this pixel (surface) from the light
	float depthFromLight = input.posForShadow.z / input.posForShadow.w;

	// Sample the shadow map with our comparison sampler so that
	// it does the comparison of the interpolated pixels for us!
	float shadowAmount = 0;
    uint pixelCount = shadowSmooth * 2 + 1;
    pixelCount *= pixelCount;
    for (int x = -shadowSmooth; x <= shadowSmooth; x++)
    {
        for (int y = -shadowSmooth; y <= shadowSmooth; y++)
        {
            shadowAmount += ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV + float2(x / shadowRes, y / shadowRes), depthFromLight - .01f) / pixelCount;
        }
    }

	float3 finalColor = float3(0,0,0);
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			finalColor += BasicDirectLight(input.normal, lights[i], cameraPos, input.worldPos, surfaceColor, shinniness, roughness, shadowAmount);
			break;
		case LIGHT_TYPE_POINT:
			finalColor += BasicPointLight(input.normal, lights[i], cameraPos, input.worldPos, surfaceColor, shinniness, roughness, 1);
			break;
		case LIGHT_TYPE_SPOT:
			finalColor += BasicSpotLight(input.normal, lights[i], cameraPos, input.worldPos, surfaceColor, shinniness, roughness, 1);
			break;
		}
	}

	return float4(finalColor,0);

}