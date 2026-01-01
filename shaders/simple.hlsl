// Vertex Shader
struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
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
    return output;
}

// Fragment/Pixel Shader
struct PSInput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

[shader("pixel")]
float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.color, 1.0);
}
