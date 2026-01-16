#include <resource_heap.hlsl>

struct PushConstants
{
	column_major float4x4 cameraWorldMatrix;
	RWTextureHandle outputTexture;
	AccelerationStructureHandle sceneBVH;
};

VK_PUSH_CONST_ATTR
PushConstants pushConstants : register(b0);

struct RayPayload
{
	float3 color;
};

[shader("raygeneration")]
void RayGenMain()
{
	uint2 dispatchDim = DispatchRaysDimensions().xy;
	uint2 pixelCoord = DispatchRaysIndex().xy;
	float2 uv = (pixelCoord + 0.5) / float2(dispatchDim);
	float2 ndc = uv * 2.0 - 1.0;
	float3 rayDir = normalize(float3(ndc.x, ndc.y, -1.0));
	float3 rayOrigin = float3(0.0, 0.0, 0.0);

	RayPayload payload;
	payload.color = float3(0.0, 0.0, 0.0);

	AccelerationStructureHandle sceneBVH = pushConstants.sceneBVH;
	RWTextureHandle outputTexture = pushConstants.outputTexture;

	RayDesc ray;
	ray.Origin = rayOrigin;
	ray.Direction = rayDir;
	ray.TMin = 0.0;
	ray.TMax = 10000.0;

	//sceneBVH.TraceRayKHR<RayPayload>(
	//	RAY_FLAG_NONE,
	//	0xFF,
	//	0,1,0,
	//	ray, payload
	//);
	outputTexture.Store2D(pixelCoord, float4(1.0, 0.0, 0.0, 1.0) );
}
