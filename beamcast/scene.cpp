#include "scene.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "../engine/image_utils.hpp"
#include "../engine/buffer_utils.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "utils.hpp"
#include "materials.hpp"

namespace beamcast {
MeshObject::MeshObject(uint32_t meshIndex, const rapidjson::Value &obj) : meshIndex(meshIndex) {
	transform = glm::identity<glm::mat4>();

	if (obj.FindMember("material_index") != obj.MemberEnd()) {
		materialIndex = obj["material_index"].GetUint() + 1;
	} else {
		materialIndex = 0;	   // Default to first material if not specified TODO: bad
	}

	if (obj.FindMember("transform") != obj.MemberEnd()) {
		const auto &t = obj["transform"].GetArray();
		if (t.Size() != 16) throw std::runtime_error("wrong number of values in matrix");
		transform = glm::mat4(t[0].GetFloat(), t[1].GetFloat(), t[2].GetFloat(), t[3].GetFloat(), t[4].GetFloat(),
							  t[5].GetFloat(), t[6].GetFloat(), t[7].GetFloat(), t[8].GetFloat(), t[9].GetFloat(),
							  t[10].GetFloat(), t[11].GetFloat(), t[12].GetFloat(), t[13].GetFloat(), t[14].GetFloat(),
							  t[15].GetFloat());
		// glm is column-major, JSON is row-major
		// and the JSON is in blender coordinate system (Z up), we want Y up
		std::swap(transform[1], transform[2]);
		transform = glm::transpose(transform);

		// inverseTransform = invert(transform);

		// isIdentity = transform == identity<float, 4>();
	}
}

GPUMeshObject MeshObject::getGPU() const {
	GPUMeshObject gpuObj = {
		.materialIndex = materialIndex,
		.meshIndex	   = meshIndex,
	};
	dbLog(dbg::LOG_DEBUG, "MeshObject GPU material index: ", gpuObj.materialIndex);
	return gpuObj;
}

Scene::Scene(nri::NRI &nri, nri::CommandQueue &q, const std::string_view &filename) : nri(&nri) {
	rapidjson::Document doc;
	scenePath = filename;
	std::ifstream file(filename.data());
	if (!file.is_open()) { throw std::runtime_error(std::format("Failed to open scene file: {}", filename)); }
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	rapidjson::ParseResult ok = doc.Parse(str.c_str());
	if (!ok) {
		auto ind = ok.Offset();
		dbLog(dbg::LOG_ERROR, "JSON parse error: ", GetParseError_En(ok.Code()), " (", ok.Offset(),
			  ")\n"
			  "In file: ",
			  filename,
			  "\n"
			  "Context: ",
			  str.substr(ind > 20 ? ind - 20 : 0, 40));

		throw std::runtime_error(std::format("Failed to parse scene file: {} Error: {} Offset: {}", filename,
											 GetParseError_En(ok.Code()), ok.Offset()));
	}

	if (doc.HasMember("meshes")) {
		auto meshesJSON = doc["meshes"].GetArray();
		for (rapidjson::SizeType i = 0; i < meshesJSON.Size(); ++i) {
			meshes.emplace_back(nri, q, meshesJSON[i]);
		}
	}

	auto objectsJSON = doc["objects"].GetArray();
	for (rapidjson::SizeType i = 0; i < objectsJSON.Size(); ++i) {
		auto &obj = objectsJSON[i];
		if (obj.HasMember("ref")) {
			uint32_t meshIndex = obj["ref"].GetUint();
			meshObjects.emplace_back(meshIndex, obj);
		} else {
			meshes.emplace_back(nri, q, objectsJSON[i]);
			meshObjects.emplace_back(meshes.size() - 1, obj);
		}
	}

	if (nri.supportsRayTracing()) {
		std::vector<const nri::BLAS *> blasList;
		std::vector<glm::mat3x4>	   transforms;
		for (const auto &obj : meshObjects) {
			assert(obj.meshIndex < meshes.size());
			blasList.push_back(&(meshes[obj.meshIndex].getBLAS()));
			transforms.push_back(glm::mat3x4(glm::transpose(obj.transform)));
		}
		dbLog(dbg::LOG_INFO, "Creating TLAS with ", blasList.size(), " instances.");
		tlas		= nri.createTLAS(blasList, transforms);
		auto cmdBuf = nri.createCommandBuffer(nri.getDefaultCommandPool());
		tlas->build(*cmdBuf);
		dbLog(dbg::LOG_DEBUG, "Submitting TLAS build command buffer.");
		auto key = q.submit(*cmdBuf);
		q.wait(key);
		tlas->buildFinished();
	}

	if (doc.FindMember("textures") != doc.MemberEnd() && nri.supportsTextures()) {
		auto texturesJSON = doc["textures"].GetArray();
		dbLog(dbg::LOG_DEBUG, "Found ", texturesJSON.Size(), " textures in scene file.");
		for (const auto &j : texturesJSON) {
			auto obj   = j.GetObject();
			bool found = false;
			if (obj["type"].GetString() == std::string_view("albedo")) {
				// textures.emplace_back(new ConstantTexure(obj));
			} else if (obj["type"].GetString() == std::string_view("checker")) {
				// textures.emplace_back(new CheckerTexture(obj));
			} else if (obj["type"].GetString() == std::string_view("edges")) {
				// textures.emplace_back(new EdgeTexture(obj));
			} else if (obj["type"].GetString() == std::string_view("bitmap")) {
				std::string filename = obj["file_path"].GetString();
#ifdef _WIN32
				if (filename.starts_with("/home/")) {	  // hack to fix absolute paths from linux exports
					dbLog(dbg::LOG_WARNING, "Texture file path '", filename,
						  "' appears to be an absolute path from a Linux system. Attempting to fix path.");
					auto diskPos = filename.find("/D/");
					filename	 = "D:" + filename.substr(diskPos + 2);
				}
#endif
				auto sceneFolder = scenePath.parent_path();
				if (sceneFolder.empty()) { sceneFolder = std::filesystem::current_path(); }
				auto fullPath = std::filesystem::path(std::string_view(filename));
				if (fullPath.is_relative()) fullPath = sceneFolder / fullPath;

				dbLog(dbg::LOG_INFO, "Loading texture '", obj["name"].GetString(), "' from file: ", fullPath.string());
				auto [image, memory] = createImage2D(fullPath.string(), nri, q);
				textures.emplace_back(std::move(image), image->createTextureView());
				memoryAllocations.push_back(std::move(memory));
				found = true;
			} else {
				throw std::runtime_error(std::format("Unknown texture type: {}", obj["type"].GetString()));
			}

			if (found) {
				textureMap[obj["name"].GetString()] = textures.back().view.get();
			} else dbLog(dbg::LOG_WARNING, "Texture '", obj["name"].GetString(), "' not added to texture map.");
		}
	}

	materials.emplace_back(std::make_unique<DiffuseMaterial>(glm::vec3(1.0)));
	materials.back()->smooth = false;
	if (doc.FindMember("materials") == doc.MemberEnd()) {
		dbLog(dbg::LOG_WARNING, "No materials found in scene file, using default materials.");
	} else {
		auto materialsJSON = doc["materials"].GetArray();
		for (const auto &j : materialsJSON) {
			if (j["type"].GetString() == std::string_view("diffuse")) {
				materials.emplace_back(std::make_unique<DiffuseMaterial>(j, *this));
			} else if (j["type"].GetString() == std::string_view("reflective")) {
				materials.emplace_back(std::make_unique<ReflectiveMaterial>(j));
			} else if (j["type"].GetString() == std::string_view("refractive")) {
				materials.emplace_back(std::make_unique<RefractiveMaterial>(j));
			} else if (j["type"].GetString() == std::string_view("constant")) {
				materials.emplace_back(std::make_unique<ConstantMaterial>(j));
			} else {
				throw std::runtime_error(std::format("Unknown material type: {}", j["type"].GetString()));
			}
		}
	}
	updateGPUBuffers(q);
}

void Scene::render(nri::CommandBuffer &commandBuffer, nri::GraphicsProgram &program, const Camera &camera) {
	for (auto &obj : meshObjects) {
		auto &mesh	   = meshes[obj.meshIndex];
		auto &material = materials[obj.materialIndex];

		mesh.bind(commandBuffer);
		glm::mat4 modelViewProjection = camera.getViewProjectionMatrix() * obj.transform;
		program.setPushConstants(commandBuffer, &modelViewProjection, sizeof(modelViewProjection), 0);

		nri::ResourceHandle textureHandle = nri::ResourceHandle::INVALID_HANDLE;
		if (nri->supportsTextures()) textureHandle = material->getTextureHandle();
		program.setPushConstants(commandBuffer, &textureHandle, sizeof(PushConstantData), sizeof(modelViewProjection));

		mesh.draw(commandBuffer, program);
	}
}

void Scene::drawMesh(nri::CommandBuffer &commandBuffer, nri::GraphicsProgram &program, const Camera &camera,
					 const Mesh &mesh, const glm::mat4 &modelMatrix, nri::ResourceHandle textureHandle) const {
	mesh.bind(commandBuffer);

	glm::mat4 modelViewProjection = camera.getViewProjectionMatrix() * modelMatrix;
	program.setPushConstants(commandBuffer, &modelViewProjection, sizeof(modelViewProjection), 0);

	if (!nri->supportsTextures()) textureHandle = nri::ResourceHandle::INVALID_HANDLE;
	program.setPushConstants(commandBuffer, &textureHandle, sizeof(PushConstantData), sizeof(modelViewProjection));

	mesh.draw(commandBuffer, program);
}

nri::ImageView *Scene::getTexture(const std::string_view &name) const {
	auto it = textureMap.find(name);
	if (it != textureMap.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

void Scene::updateGPUBuffers(nri::CommandQueue &queue) {
	materialsBuffer =
		nri->createBuffer(sizeof(GPUMaterial) * materials.size(),
						  nri::BufferUsage::BUFFER_USAGE_STORAGE | nri::BufferUsage::BUFFER_USAGE_TRANSFER_DST);
	meshObjectsBuffer =
		nri->createBuffer(sizeof(GPUMeshObject) * meshObjects.size(),
						  nri::BufferUsage::BUFFER_USAGE_STORAGE | nri::BufferUsage::BUFFER_USAGE_TRANSFER_DST);

	meshesBuffer = nri->createBuffer(sizeof(GPUMesh) * meshes.size(), nri::BufferUsage::BUFFER_USAGE_STORAGE |
																		  nri::BufferUsage::BUFFER_USAGE_TRANSFER_DST);

	auto [offsets, memreq] =
		beamcast::getBufferOffsets({materialsBuffer.get(), meshObjectsBuffer.get(), meshesBuffer.get()});
	auto mem =
		beamcast::allocateBindMemory(*nri, offsets, memreq.setTypeRequest(nri::MemoryTypeRequest::MEMORY_TYPE_DEVICE),
									 {materialsBuffer.get(), meshObjectsBuffer.get(), meshesBuffer.get()});
	memoryAllocations.push_back(std::move(mem));

	auto stagingBuffer = nri->createBuffer(memreq.size, nri::BufferUsage::BUFFER_USAGE_TRANSFER_SRC);
	auto stagingMem =
		beamcast::allocateBindMemory(*nri, {stagingBuffer.get()}, nri::MemoryTypeRequest::MEMORY_TYPE_UPLOAD);
	{
		void		*data		  = stagingBuffer->map(0, stagingBuffer->getSize());
		GPUMaterial *gpuMaterials = (GPUMaterial *)((uint8_t *)data + offsets[0]);
		for (size_t i = 0; i < materials.size(); ++i)
			gpuMaterials[i] = materials[i]->getGPU();
		GPUMeshObject *gpuMeshObjects = (GPUMeshObject *)((uint8_t *)data + offsets[1]);
		for (size_t i = 0; i < meshObjects.size(); ++i)
			gpuMeshObjects[i] = meshObjects[i].getGPU();
		GPUMesh *gpuMeshes = (GPUMesh *)((uint8_t *)data + offsets[2]);
		for (size_t i = 0; i < meshes.size(); ++i)
			gpuMeshes[i] = meshes[i].getGPU();

		stagingBuffer->unmap();
	}

	auto cmdBuf = nri->createCommandBuffer(nri->getDefaultCommandPool());
	cmdBuf->begin();
	materialsBuffer->copyFrom(*cmdBuf, *stagingBuffer, offsets[0], 0, materialsBuffer->getSize());
	meshObjectsBuffer->copyFrom(*cmdBuf, *stagingBuffer, offsets[1], 0, meshObjectsBuffer->getSize());
	meshesBuffer->copyFrom(*cmdBuf, *stagingBuffer, offsets[2], 0, meshesBuffer->getSize());
	cmdBuf->end();
	auto key = queue.submit(*cmdBuf);
	queue.wait(key);
}

nri::PushConstantRange Scene::getPushConstantRange() {
	auto cameraRange = Camera::getPushConstantRange();
	return {cameraRange.offset, uint32_t(cameraRange.size + sizeof(PushConstantData))};
}

nri::TLAS	&Scene::getTLAS() { return *tlas; }
nri::Buffer &Scene::getMaterialsBuffer() { return *materialsBuffer; }
nri::Buffer &Scene::getMeshObjectsBuffer() { return *meshObjectsBuffer; }
nri::Buffer &Scene::getMeshesBuffer() { return *meshesBuffer; }
}	  // namespace beamcast
