#include "materials.hpp"
#include <rapidjson/document.h>
#include "scene.hpp"

DiffuseMaterial::DiffuseMaterial(const rapidjson::Value &obj, const Scene &scene) : Material(obj, true, true) {
	const auto &albedoJSON = obj["albedo"];
	if (albedoJSON.IsArray()) {
		const auto &colorJSON = albedoJSON.GetArray();
		this->albedoColor =
			glm::vec3{colorJSON[0].GetFloat(), colorJSON[1].GetFloat(), colorJSON[2].GetFloat()};
		this->albedo = nullptr;
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
