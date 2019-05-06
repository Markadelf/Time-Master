

cbuffer externalData : register(b0)
{
	int debugWireframe;
};

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
	float4 color		: TEXCOORD1;
};

// Textures and such
Texture2D particle		: register(t0);
SamplerState trilinear	: register(s0);


// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Sample texture and combine with input color
	float4 color = particle.Sample(trilinear, input.uv) * input.color;

	// Return either particle color or white (for debugging)
	return lerp(color, float4(1, 1, 1, 0.25f), debugWireframe);
}