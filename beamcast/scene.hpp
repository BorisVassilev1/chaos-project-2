#pragma once

#include <vector>


#include "mesh.hpp"

class Scene {

	std::vector<Mesh> meshes;

   public:
	Scene() = default;
	Scene(NRI &nri, NRICommandQueue &q,const std::string_view &filename);

	void render(NRICommandBuffer &commandBuffer, NRIGraphicsProgram &program) {
		for (auto &mesh : meshes) {
			mesh.bind(commandBuffer);
			mesh.draw(commandBuffer, program);
		}
	}
};
