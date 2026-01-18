#include "materials.hpp"
#include <rapidjson/document.h>
#include "scene.hpp"

namespace beamcast {
DiffuseMaterial::DiffuseMaterial(const rapidjson::Value &obj, const Scene &scene) : Material(obj, true, true) {
	const auto &albedoJSON = obj["albedo"];
	if (albedoJSON.IsArray()) {
		const auto &colorJSON = albedoJSON.GetArray();
		this->albedoColor	  = glm::vec3{colorJSON[0].GetFloat(), colorJSON[1].GetFloat(), colorJSON[2].GetFloat()};
		this->albedo		  = nullptr;
		return;
	} else if (albedoJSON.IsString()) {
		this->albedoColor			 = glm::vec3(0, 0, 0);
		std::string_view textureName = obj["albedo"].GetString();
		albedo						 = scene.getTexture(textureName);
	} else {
		throw std::runtime_error("Invalid albedo type for DiffuseMaterial");
	}
	doubleSided = false;
}

GPUMaterial DiffuseMaterial::getGPU() const {
	GPUMaterial gpuMat = {
		.albedo		   = glm::vec4(albedoColor, 1.0f),
		.albedoTexture = albedo ? albedo->getHandle() : nri::ResourceHandle::INVALID_HANDLE,
	};
	return gpuMat;
}

GPUMaterial ReflectiveMaterial::getGPU() const {
	GPUMaterial gpuMat = {
		.albedo		   = glm::vec4(albedo, 0.0f),
		.albedoTexture = nri::ResourceHandle::INVALID_HANDLE,
	};
	return gpuMat;
}

GPUMaterial RefractiveMaterial::getGPU() const {
	GPUMaterial gpuMat = {
		.albedo		   = glm::vec4(1.0f - absorbtion, 0.0f),
		.albedoTexture = nri::ResourceHandle::INVALID_HANDLE,
	};
	return gpuMat;
}

GPUMaterial ConstantMaterial::getGPU() const {
	GPUMaterial gpuMat = {
		.albedo		   = glm::vec4(albedo, 0.0f),
		.albedoTexture = nri::ResourceHandle::INVALID_HANDLE,
	};
	return gpuMat;
}

}	  // namespace beamcast
