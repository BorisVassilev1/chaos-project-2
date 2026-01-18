//#include <resource_heap.hlsl>
#include <common_structs.hlsl>

struct PushConstants
{
	column_major float4x4 cameraWorldMatrix;
	float fovY;
	RWTextureHandle outputTexture;
	AccelerationStructureHandle sceneBVH;
	ArrayBufferHandle materials;
	ArrayBufferHandle meshObjects;
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
	ndc.x *= (float(dispatchDim.x) / float(dispatchDim.y));
	float3 rayDir = normalize(float3(ndc.x, ndc.y, -1.0 / tan(pushConstants.fovY * 0.5)));
	rayDir = mul((float3x3)pushConstants.cameraWorldMatrix, rayDir);
	float3 rayOrigin = float3(
		pushConstants.cameraWorldMatrix[0].w,
		pushConstants.cameraWorldMatrix[1].w, 
		pushConstants.cameraWorldMatrix[2].w
	);

	RayPayload payload;
	payload.color = float3(0.0, 0.0, 0.0);

	AccelerationStructureHandle sceneBVH = pushConstants.sceneBVH;
	RWTextureHandle outputTexture = pushConstants.outputTexture;

	RayDesc ray;
	ray.Origin = rayOrigin;
	ray.Direction = rayDir;
	ray.TMin = 0.0;
	ray.TMax = 10000.0;

	sceneBVH.TraceRayKHR<RayPayload>(
		RAY_FLAG_NONE,
		0xFF,
		0,1,0,
		ray, payload
	);
	outputTexture.Store2D(pixelCoord, float4(payload.color, 1.0) );
}

[shader("miss")]
void MissMain(inout RayPayload payload)
{
	payload.color = float3(0.0, 0.0, 0.0);
}

[shader("closesthit")]
void ClosestHitMain(inout RayPayload payload, BuiltInTriangleIntersectionAttributes attr)
{
	float3 colors[3] = {
		float3(1.0, 0.0, 0.0),
		float3(0.0, 1.0, 0.0),
		float3(0.0, 0.0, 1.0)
	};
	float3 position = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
	//payload.color = position * .1f;
	uint materialIndex = pushConstants.meshObjects.Load<GPUMeshObject>(InstanceID()).materialIndex;
	GPUMaterial material = pushConstants.materials.Load<GPUMaterial>(materialIndex);
	if(material.albedoTexture.IsValid()) {
		float2 texCoord = attr.barycentrics.xy;
		float4 texColor = material.albedoTexture.Sample2D<float4>(texCoord, 0);
		payload.color = texColor.rgb;
	} else {
		payload.color = material.albedo;
	}

	//payload.color = colors[materialIndex % 3];
	//payload.color = colors[InstanceID() % 3];

}
