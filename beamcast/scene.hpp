#pragma once

#include <vector>

#include "camera.hpp"
#include "materials.hpp"
#include "mesh.hpp"
#include "utils.hpp"

class MeshObject {
   public:
	uint32_t  meshIndex;
	uint32_t  materialIndex;
	glm::mat4 transform;

	MeshObject(uint32_t meshIndex, const rapidjson::Value &meshObjectData);
};

class Scene {
	std::vector<Mesh>		meshes;
	std::vector<MeshObject> meshObjects;
	std::vector<std::unique_ptr<Material>>	materials;

	std::vector<std::unique_ptr<NRIImage2D>>	  textures;
	std::unordered_map<std::string, NRIImage2D *, string_hash, std::equal_to<>> textureMap;

	std::filesystem::path scenePath;

	std::vector<std::unique_ptr<NRIAllocation>>	memoryAllocations;

   public:
	Scene() = default;
	Scene(NRI &nri, NRICommandQueue &q, const std::string_view &filename);

	void render(NRICommandBuffer &commandBuffer, NRIGraphicsProgram &program, const Camera &camera);

	NRIImage2D *getTexture(const std::string_view &name) const;
	
	struct PushConstantData {
		NRIResourceHandle textureHandle;
	};

	static NRI::PushConstantRange getPushConstantRange();
};
