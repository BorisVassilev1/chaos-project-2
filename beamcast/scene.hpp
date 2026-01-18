#pragma once

#include <vector>

#include "camera.hpp"
#include "materials.hpp"
#include "mesh.hpp"
#include "utils.hpp"
#include "../shaders/common_structs.hlsl"

namespace beamcast {
class MeshObject {
   public:
	uint32_t  meshIndex;
	uint32_t  materialIndex;
	glm::mat4 transform;

	MeshObject(uint32_t meshIndex, const rapidjson::Value &meshObjectData);
	GPUMeshObject getGPU() const;
};

class Scene {
	nri::NRI							  *nri = nullptr;
	std::vector<Mesh>					   meshes;
	std::vector<MeshObject>				   meshObjects;
	std::vector<std::unique_ptr<Material>> materials;

	std::vector<nri::ImageAndViewPtr>												textures;
	std::unordered_map<std::string, nri::ImageView *, string_hash, std::equal_to<>> textureMap;

	std::filesystem::path scenePath;

	std::vector<std::unique_ptr<nri::Allocation>> memoryAllocations;

	std::unique_ptr<nri::Buffer> materialsBuffer;		// array of GPUMaterial
	std::unique_ptr<nri::Buffer> meshObjectsBuffer;		// array of GPUMeshObject
	std::unique_ptr<nri::Buffer> meshesBuffer;			// array of GPUMesh

	std::unique_ptr<nri::TLAS> tlas;

   public:
	Scene() = default;
	Scene(nri::NRI &nri, nri::CommandQueue &q, const std::string_view &filename);

	void render(nri::CommandBuffer &commandBuffer, nri::GraphicsProgram &program, const Camera &camera);
	void drawMesh(nri::CommandBuffer &commandBuffer, nri::GraphicsProgram &program, const Camera &camera,
				  const Mesh &meshIndex, const glm::mat4 &modelMatrix,
				  nri::ResourceHandle textureHandle = nri::ResourceHandle::INVALID_HANDLE) const;

	nri::ImageView *getTexture(const std::string_view &name) const;

	void updateGPUBuffers(nri::CommandQueue &queue);

	struct PushConstantData {
		nri::ResourceHandle textureHandle;
	};

	static nri::PushConstantRange getPushConstantRange();
	nri::TLAS					 &getTLAS();
	nri::Buffer					 &getMaterialsBuffer();
	nri::Buffer					 &getMeshObjectsBuffer();
	nri::Buffer					 &getMeshesBuffer();
};
}	  // namespace beamcast
