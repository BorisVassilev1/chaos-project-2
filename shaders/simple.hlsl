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

[shader("vertex")]
VSOutput VSMain(VSInput input)
{

    VSOutput output;
    output.position = float4(input.position.xyz, 1.0);
#if defined(DX12)
    output.position.y = -output.position.y; // Invert Y for DirectX 12
#endif
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
