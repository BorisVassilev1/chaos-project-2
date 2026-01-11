// Vertex Shader
struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0_;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

class ResourceHandle
{
	uint index;

	uint GetIndex() { return index & 0x1FFFFFFF; }
	bool IsWritable() { return (index & 0x80000000) != 0; }
	uint GetType() { return (index >> 29) & 0x3; }
};


struct PushConstants
{
	column_major float4x4 modelViewProjection;
	uint32_t texture;
};

[[vk::push_constant]]
PushConstants pushConstants : register(b0);


[shader("vertex")]
VSOutput VSMain(VSInput input)
{
	float4 pos = float4(input.position, 1.0);
	pos = mul(pushConstants.modelViewProjection, pos);
	float3 normal = mul((float3x3)pushConstants.modelViewProjection, input.normal);

    VSOutput output;
	output.position = pos;
    output.color = input.color;
	output.normal = normal;
	output.texCoord = input.texCoord;
    return output;
}

// Fragment/Pixel Shader
struct PSInput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

[[vk::binding(0, 0)]]
Sampler2D<float4> textures[100] : register(t0);

[shader("pixel")]
float4 PSMain(PSInput input) : SV_TARGET
{
	float4 texColor;
	if(pushConstants.texture != 0xFFFFFFFF) {
		uint textureIndex = pushConstants.texture & 0x1FFFFFFF;
		texColor = textures[textureIndex].Sample(input.texCoord);
	} else {
		texColor = float4(1.0, 1.0, 1.0, 1.0);
	}

	// flip normal if looking from behind
	if(dot(normalize(input.normal), float3(0.0, 0.0, -1.0)) < 0.0) {
		input.normal = -input.normal;
	}

	// simple lighting with normal and camera position
	float3 lightDir = normalize(float3(0.0, 0.0, -1.0));
	float diff = max(dot(normalize(input.normal), lightDir), 0.0);
	texColor.xyz *= saturate(diff * 0.8f + 0.2f);

    return float4(texColor.xyz, 1.0);
}
