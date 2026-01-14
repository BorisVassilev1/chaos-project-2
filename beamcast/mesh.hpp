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
	std::unique_ptr<NRIBLAS>   bottomLevelAS;

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
	const NRIBLAS				   &getBLAS() const;
};

class TriangleMesh : public Mesh {
   public:
	TriangleMesh(NRI &nri, NRICommandQueue &q);
};
