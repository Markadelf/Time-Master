
// Constant buffer for C++ data being passed in
cbuffer externalData : register(b0)
{
    matrix view;
    matrix projection;

	int startIndex;

	float3 acceleration;

	float4 startColor;
	float4 endColor;

	float startSize;
	float endSize;

	float lifetime;
	float currentTime;

    float3 emitterPosition;

    float startTime;
    float endTime;
};

struct Particle
{
	float SpawnTime;
	float3 StartPosition;

	float3 StartVelocity;
	float RotationStart;

	float RotationEnd;
	float3 padding;
};
// Describes individual vertex data
//struct VertexShaderInput
//{
//    float3 position		: POSITION;
//    float2 uv			: TEXCOORD;
//    float4 color		: COLOR;
//};

// Defines the output data of our vertex shader
struct VertexToPixel
{
    float4 position		: SV_POSITION;
    float2 uv           : TEXCOORD0;
	float4 color		: TEXCOORD1;
};
StructuredBuffer<Particle> ParticleData : register(t0);
// The entry point for our vertex shader
VertexToPixel main(uint id : SV_VertexID)
{
	// Set up output
	VertexToPixel output;

	// Get id info
	uint particleID = id / 4; // Every group of 4 verts are ONE particle!
	uint cornerID = id % 4;

	// Grab one particle
	Particle p = ParticleData.Load(particleID + startIndex);

	// Calc the age percent
    if (currentTime < startTime + p.SpawnTime || currentTime > endTime)
    {
        output.position = float4(-1, -1, -1, -1);
        return output;
    }
	float t = currentTime - p.SpawnTime - startTime;
	t = ((t % lifetime)) % lifetime;
    float agePercent = t / lifetime; // The "age percent": 0 - 1

	//float agePercent1 = t / lifetime;
	

	// Calc anything based on time
	float3 pos = acceleration * t * t / 2.0f + p.StartVelocity * t + p.StartPosition + emitterPosition;
	float4 color = lerp(startColor, endColor, agePercent);
	float size = lerp(startSize, endSize, agePercent);
	float rotation = lerp(p.RotationStart, p.RotationEnd, agePercent);

	// Offsets for smaller triangles
	float2 offsets[4];
	offsets[0] = float2(-1.0f, +1.0f);  // TL
	offsets[1] = float2(+1.0f, +1.0f);  // TR
	offsets[2] = float2(+1.0f, -1.0f);  // BR
	offsets[3] = float2(-1.0f, -1.0f);  // BL

	// Handle rotation - get sin/cos and build a rotation matrix
	float s, c;
	sincos(rotation, s, c); // One function that calc's sin and cos
	float2x2 rot =
	{
		c, s,
		-s, c
	};

	// Rotate the offset for this corner
	float2 rotatedOffset = mul(offsets[cornerID], rot);

	// Offset the position based on the camera's right and up vectors
	pos += float3(view._11, view._21, view._31) * rotatedOffset.x * size;
	pos += float3(view._12, view._22, view._32) * rotatedOffset.y * size;

	// Calculate output position
	matrix viewProj = mul(view, projection);
	output.position = mul(float4(pos, 1.0f), viewProj);

	float2 uvs[4];
	uvs[0] = float2(0, 0);  // TL
	uvs[1] = float2(1, 0);  // TR
	uvs[2] = float2(1, 1);  // BR
	uvs[3] = float2(0, 1);  // BL

	// Pass uv through
	output.uv = saturate(uvs[cornerID]);
	output.color = color;

	return output;
}