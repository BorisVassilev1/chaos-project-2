#include "mesh.hpp"

Mesh::Mesh(NRI &nri, NRICommandQueue &q, std::span<float> vertices, std::span<float> colors, std::span<float> normals,
		   std::span<float> texCoords, std::span<uint32_t> indices) {
	init(nri, q, vertices, colors, normals, texCoords, indices);
}

void Mesh::init(NRI &nri, NRICommandQueue &q, std::span<float> vertices, std::span<float> colors,
				std::span<float> normals, std::span<float> texCoords, std::span<uint32_t> indices) {
	if (vertices.size() % 3 != 0) {
		dbLog(dbg::LOG_ERROR, "Vertices size: ", vertices.size(), " must be a multiple of 3");
		throw std::runtime_error("Vertices size must be a multiple of 3");
	}
	if (colors.size() % 3 != 0) {
		dbLog(dbg::LOG_ERROR, "Colors size: ", colors.size(), " must be a multiple of 3");
		throw std::runtime_error("Colors size must be a multiple of 3");
	}
	if (normals.size() % 3 != 0) {
		dbLog(dbg::LOG_ERROR, "Normals size: ", normals.size(), " must be a multiple of 3");
		throw std::runtime_error("Normals size must be a multiple of 3");
	}
	if (texCoords.size() % 2 != 0) {
		dbLog(dbg::LOG_ERROR, "TexCoords size: ", texCoords.size(), " must be a multiple of 2");
		throw std::runtime_error("TexCoords size must be a multiple of 2");
	}
	if (vertices.size() / 3 != colors.size() / 3 || vertices.size() / 3 != normals.size() / 3 ||
		vertices.size() / 3 != texCoords.size() / 2) {
		dbLog(dbg::LOG_ERROR, "Vertices count: ", vertices.size() / 3, ", Colors count: ", colors.size() / 3,
			  ", Normals count: ", normals.size() / 3, ", TexCoords count: ", texCoords.size() / 2,
			  " must be the same");
		throw std::runtime_error("Vertices, colors, normals and texCoords must have the same number of vertices");
	}
	if (indices.size() % 3 != 0) {
		dbLog(dbg::LOG_ERROR, "Indices size: ", indices.size(), " must be a multiple of 3");
		throw std::runtime_error("Indices size must be a multiple of 3");
	}
	vertexCount = vertices.size() / 3;
	indexCount	= indices.size();

	vertexAttributes = nri.createBuffer(vertexCount * (3 + 3 + 3 + 2) * sizeof(float),
										NRI::BUFFER_USAGE_VERTEX | NRI::BUFFER_USAGE_TRANSFER_DST);
	indexBuffer =
		nri.createBuffer(indices.size() * sizeof(uint32_t), NRI::BUFFER_USAGE_INDEX | NRI::BUFFER_USAGE_TRANSFER_DST);

	auto [offsets, memReq] = getBufferOffsets({vertexAttributes.get(), indexBuffer.get()});
	memory				   = allocateBindMemory(nri, {vertexAttributes.get(), indexBuffer.get()},
												NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE);

	auto uploadBuffer		= nri.createBuffer(memReq.size, NRI::BUFFER_USAGE_TRANSFER_SRC);
	auto uploadBufferMemory = allocateBindMemory(nri, {uploadBuffer.get()}, NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD);

	{
		void	*data		   = uploadBuffer->map(0, uploadBuffer->getSize());
		dbLog(dbg::LOG_WARNING, "vertexattributes offset: ", vertexAttributes->getOffset(),
			  " size: ", vertexAttributes->getSize());
		dbLog(dbg::LOG_WARNING, "index offset: ", indexBuffer->getOffset(), " size: ", indexBuffer->getSize());
		uint8_t *vertexDataPtr = static_cast<uint8_t *>(data) + vertexAttributes->getOffset();
		for (std::size_t i = 0; i < vertexCount; i++) {
			std::memcpy(vertexDataPtr, &vertices[i * 3], 3 * sizeof(float));
			vertexDataPtr += 3 * sizeof(float);
			std::memcpy(vertexDataPtr, &colors[i * 3], 3 * sizeof(float));
			vertexDataPtr += 3 * sizeof(float);
			std::memcpy(vertexDataPtr, &normals[i * 3], 3 * sizeof(float));
			vertexDataPtr += 3 * sizeof(float);
			std::memcpy(vertexDataPtr, &texCoords[i * 2], 2 * sizeof(float));
			vertexDataPtr += 2 * sizeof(float);
		}
		std::memcpy(static_cast<uint8_t *>(data) + indexBuffer->getOffset(), indices.data(),
					indices.size() * sizeof(uint32_t));
		uploadBuffer->unmap();
	}

	auto cmdBuffer = nri.createCommandBuffer(nri.getDefaultCommandPool());
	vertexAttributes->copyFrom(*cmdBuffer, *uploadBuffer, vertexAttributes->getOffset(), 0,
							   vertexAttributes->getSize());
	indexBuffer->copyFrom(*cmdBuffer, *uploadBuffer, indexBuffer->getOffset(), 0, indexBuffer->getSize());
	cmdBuffer->end();

	auto key = q.submit(*cmdBuffer);
	q.wait(key);
}

Mesh::Mesh(NRI &nri, NRICommandQueue &q, const rapidjson::Value &obj) {
	auto &verticesJSON = obj["vertices"];
	assert(verticesJSON.IsArray());

	std::vector<float>	  vertices;
	std::vector<float>	  texCoords;
	std::vector<float>	  normals;
	std::vector<uint32_t> indices;

	vertices.reserve(verticesJSON.Capacity() / 3);
	for (unsigned int i = 0; i < verticesJSON.Capacity(); i += 3) {
		if (i + 2 >= verticesJSON.Capacity()) {
			throw std::runtime_error("Invalid number of vertices in triangle object");
		}
		vertices.push_back(verticesJSON[i].GetFloat());
		vertices.push_back(verticesJSON[i + 1].GetFloat());
		vertices.push_back(verticesJSON[i + 2].GetFloat());
	}

	texCoords.reserve(vertices.size());
	if (obj.FindMember("uvs") == obj.MemberEnd()) {
		texCoords.resize(vertices.size(), 0.0f);
		// dbLog(dbg::LOG_WARNING, "No texture coordinates found in triangle object.");
	} else {
		auto &texCoordsJSON = obj["uvs"];
		assert(texCoordsJSON.Capacity() == verticesJSON.Capacity());
		for (unsigned int i = 0; i < texCoordsJSON.Capacity(); i += 3) {
			if (i + 2 >= texCoordsJSON.Capacity()) {
				throw std::runtime_error("Invalid number of texture coordinates in triangle object");
			}
			texCoords.push_back(texCoordsJSON[i].GetFloat());
			texCoords.push_back(texCoordsJSON[i + 1].GetFloat());
			// texCoords.push_back(texCoordsJSON[i + 2].GetFloat());
		}
	}

	auto &indicesJSON = obj["triangles"];
	if (indicesJSON.Capacity() % 3 != 0) {
		throw std::runtime_error("Indices must be a multiple of 3 for triangle objects");
	}

	indices.reserve(indicesJSON.Capacity() / 3);
	for (unsigned int i = 0; i < indicesJSON.Capacity(); i += 3) {
		if (i + 2 >= indicesJSON.Capacity()) {
			throw std::runtime_error("Invalid number of indices in triangle object");
		}
		unsigned int idx0 = indicesJSON[i].GetUint();
		unsigned int idx1 = indicesJSON[i + 1].GetUint();
		unsigned int idx2 = indicesJSON[i + 2].GetUint();

		if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size()) {
			throw std::runtime_error("Index out of bounds in triangle object");
		}

		indices.push_back(idx0);
		indices.push_back(idx1);
		indices.push_back(idx2);
	}
	normals.resize(vertices.size(), 0.0f);

	auto colors = std::vector<float>(vertices.size(), 1.0f);
	// dbLog(dbg::LOG_DEBUG, "Mesh created with ", vertices.size(), " vertices and ", indices.size(), " triangles.");
	init(nri, q, vertices, colors, normals, texCoords, indices);
}

void Mesh::bind(NRICommandBuffer &cmdBuffer) {
	vertexAttributes->bindAsVertexBuffer(cmdBuffer, 0, 0, (3 + 3 + 3 + 2) * sizeof(float));
	indexBuffer->bindAsIndexBuffer(cmdBuffer, 0, NRI::IndexType::INDEX_TYPE_UINT32);
}

void Mesh::draw(NRICommandBuffer &cmdBuffer, NRIGraphicsProgram &program) {
	program.drawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
}

std::vector<NRI::VertexBinding> Mesh::getVertexBindings() const {
	return {
		{0,
		 (3 + 3 + 3 + 2) * sizeof(float),
		 NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX,
		 {
			 {0, NRI::Format::FORMAT_R32G32B32_SFLOAT, 0, NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX, "POSITION"},
			 {1, NRI::Format::FORMAT_R32G32B32_SFLOAT, 12, NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX, "COLOR"},
			 {2, NRI::Format::FORMAT_R32G32B32_SFLOAT, 24, NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX, "NORMAL"},
			 {3, NRI::Format::FORMAT_R32G32_SFLOAT, 36, NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX, "TEXCOORD0_"},
		 }}};
}
