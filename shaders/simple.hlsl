// Vertex Shader
struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};


struct PushConstants
{
	column_major float4x4 modelViewProjection;
};

[[vk::push_constant]]
PushConstants pushConstants;


[shader("vertex")]
VSOutput VSMain(VSInput input)
{
	float4 pos = float4(input.position, 1.0);
	pos = mul(pushConstants.modelViewProjection, pos);

    VSOutput output;
	output.position = pos;
    output.color = input.color;
	output.normal = input.normal;
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

[shader("pixel")]
float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.texCoord, 1.0);
}
