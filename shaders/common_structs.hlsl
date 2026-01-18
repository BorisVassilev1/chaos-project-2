#ifndef SHADER
#pragma once
#include "../native/nri.hpp"
#define float2 glm::vec2
#define float3 glm::vec3
#define float4 glm::vec4
#define float3x3 glm::mat3
#define float4x4 glm::mat4
#define uint2 glm::uvec2
#define uint3 glm::uvec3
#define uint4 glm::uvec4
#define int2 glm::ivec2
#define int3 glm::ivec3
#define int4 glm::ivec4
#define TextureHandle nri::ResourceHandle
#define RWTextureHandle nri::ResourceHandle
#define AccelerationStructureHandle nri::ResourceHandle
#define ArrayBufferHandle nri::ResourceHandle
#else // SHADER
#include <resource_heap.hlsl>
#define nri  
#endif // SHADER

struct GPUMaterial {
	float3 albedo;
	TextureHandle albedoTexture;
};

struct GPUMeshObject {
	uint materialIndex;
	uint meshIndex;
};

struct GPUMesh {
	ArrayBufferHandle vertexBuffer;
	ArrayBufferHandle indexBuffer;
	ArrayBufferHandle bottomLevelAS;
	uint32_t			vertexCount;
	uint32_t			indexCount;
};

struct GPUVertex {
	float3 position;
	float3 color;
	float3 normal;
	float2 texCoord;
};

