
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
	float2 uv			: TEXCOORD;		//XY coord
};

// Struct for lights
struct DirectionalLight
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

// External Data for 
cbuffer externalData : register(b0)
{
	DirectionalLight light[10];
	int lightAmount;
};

Texture2D diffuseTexture : register(t0);
SamplerState basicSampler : register(s0);

// Helper function for lighting calc
float4 lightCalc(float3 normal, DirectionalLight light) {
	// Correct the direction
	float3 lightDir = normalize(-light.direction);
	// Get reflection value
	float lightAmount = saturate(dot(normal, lightDir));
	// Get the light value
	return light.ambientColor + (light.diffuseColor * lightAmount);
}

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
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
	// Normalize normal
	input.normal = normalize(input.normal);
	
	float4 netLight = float4(0, 0, 0, 1);
	for (int i = 0; i < lightAmount; i++)
	{
		netLight += lightCalc(input.normal, light[i]);
	}

	return surfaceColor * netLight;
}