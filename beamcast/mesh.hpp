#pragma once
#include <memory>
#include <cstdint>
#include <span>

#include <rapidjson/document.h>

#include "../native/nri.hpp"
class Mesh {
   protected:
	std::unique_ptr<NRIAllocation> memory;

	std::unique_ptr<NRIBuffer> vertexAttributes;
	std::unique_ptr<NRIBuffer> indexBuffer;
	std::unique_ptr<NRIBLAS>	bottomLevelAS;

	uint32_t vertexCount = 0;
	uint32_t indexCount	 = 0;

	void init(NRI &nri, NRICommandQueue &q, std::span<float> vertices, std::span<float> colors,
			  std::span<float> normals, std::span<float> texCoords, std::span<uint32_t> indices);

   public:
	Mesh() {}
	Mesh(NRI &nri, NRICommandQueue &q, std::span<float> vertices, std::span<float> colors, std::span<float> normals,
		 std::span<float> texCoords, std::span<uint32_t> indices);
	Mesh(NRI &nri, NRICommandQueue &q, const rapidjson::Value &obj);

	void bind(NRICommandBuffer &cmdBuffer);
	void draw(NRICommandBuffer &cmdBuffer, NRIGraphicsProgram &program);

	std::vector<NRI::VertexBinding> getVertexBindings() const;
};

class TriangleMesh : public Mesh {
   public:
	TriangleMesh(NRI &nri, NRICommandQueue &q) : Mesh() {
		std::vector<float> vertices = {
			0.0f,  0.5f,  0.0f,		// Vertex 1 Position
			-0.5f, -0.5f, 0.0f,		// Vertex 2 Position
			0.5f,  -0.5f, 0.0f,		// Vertex 3 Position
		};
		std::vector<float> colors = {
			1.0f, 0.0f, 0.0f,	  // Vertex 1 Color
			0.0f, 1.0f, 0.0f,	  // Vertex 2 Color
			0.0f, 0.0f, 1.0f	  // Vertex 3 Color
		};
		std::vector<float>	  normals(vertices.size(), 0.0f);				// Dummy normals
		std::vector<float>	  texCoords(vertices.size() / 3 * 2, 0.0f);		// Dummy texture coordinates
		std::vector<uint32_t> indices = {0, 1, 2};

		init(nri, q, vertices, colors, normals, texCoords, indices);
	}
};
