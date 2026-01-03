#pragma once

#include <vector>

#include "camera.hpp"
#include "mesh.hpp"

class MeshObject {
   public:
	uint32_t  meshIndex;
	glm::mat4 transform;

	MeshObject(uint32_t meshIndex, const rapidjson::Value &meshObjectData);
};

class Scene {
	std::vector<Mesh>		meshes;
	std::vector<MeshObject> meshObjects;

   public:
	Scene() = default;
	Scene(NRI &nri, NRICommandQueue &q, const std::string_view &filename);

	void render(NRICommandBuffer &commandBuffer, NRIGraphicsProgram &program, const Camera &camera) {
		for (auto &obj : meshObjects) {
			auto &mesh = meshes[obj.meshIndex];

			mesh.bind(commandBuffer);
			glm::mat4 modelViewProjection = camera.getViewProjectionMatrix() * obj.transform;
			program.setPushConstants(commandBuffer, &modelViewProjection, sizeof(modelViewProjection), 0);

			mesh.draw(commandBuffer, program);
		}
	}
};
