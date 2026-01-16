#pragma once
#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include "../native/nri.hpp"

namespace beamcast {
class Scene;

class Material {
   public:
	bool smooth : 1			 = false;
	bool castsShadows : 1	 = false;
	bool receivesShadows : 1 = true;
	bool doubleSided : 1	 = false;

	Material() = default;
	Material(const rapidjson::Value &obj, bool castsShadows = true, bool receivesShadows = true)
		: castsShadows(castsShadows), receivesShadows(receivesShadows) {
		if (obj.FindMember("smooth_shading") != obj.MemberEnd()) { smooth = obj["smooth_shading"].GetBool(); }
		if (obj.FindMember("back_face_culling") != obj.MemberEnd()) {
			doubleSided = !obj["back_face_culling"].GetBool();
		}
	}

	// virtual glm::vec4 shade(const RayHit &hit, const Ray &ray, const Scene &scene, uint32_t &seed) const = 0;

	virtual ~Material() = default;

	virtual nri::ResourceHandle getTextureHandle() const { return nri::ResourceHandle::INVALID_HANDLE; }
};

class DiffuseMaterial : public Material {
   public:
	nri::ImageView *albedo;
	glm::vec3		albedoColor;

	DiffuseMaterial(const glm::vec3 &albedo) : albedo(nullptr), albedoColor(albedo) {}

	DiffuseMaterial(const rapidjson::Value &obj, const Scene &scene);
	// vec4 shade(const RayHit &hit, const Ray &, const Scene &scene, uint32_t &seed) const override;

	virtual nri::ResourceHandle getTextureHandle() const override {
		if (albedo) {
			// Assume albedo texture is bound as a sampler image at index 0
			return albedo->getHandle();
		} else {
			return nri::ResourceHandle::INVALID_HANDLE;
		}
	}
};

class ReflectiveMaterial : public Material {
   public:
	glm::vec3 albedo;

	ReflectiveMaterial(const glm::vec3 &albedo) : albedo(albedo) {}

	ReflectiveMaterial(const rapidjson::Value &obj) : Material(obj, true, true) {
		const auto &colorJSON = obj["albedo"].GetArray();
		albedo				  = glm::vec3{colorJSON[0].GetFloat(), colorJSON[1].GetFloat(), colorJSON[2].GetFloat()};
	}

	// vec4 shade(const RayHit &hit, const Ray &, const Scene &scene, uint32_t &seed) const override;
};

class RefractiveMaterial : public Material {
   public:
	glm::vec3 absorbtion;
	float	  ior;

	RefractiveMaterial(const glm::vec3 &absorbtion) : absorbtion(absorbtion) {}

	RefractiveMaterial(const rapidjson::Value &obj) : Material(obj, false, false) {
		if (obj.FindMember("ior") != obj.MemberEnd()) {
			ior = obj["ior"].GetFloat();
		} else {
			ior = 1.5f;		// Default IOR for glass
		}
		if (obj.FindMember("absorbtion") != obj.MemberEnd()) {
			const auto &colorJSON = obj["absorbtion"].GetArray();
			absorbtion = glm::vec3{colorJSON[0].GetFloat(), colorJSON[1].GetFloat(), colorJSON[2].GetFloat()};
		} else {
			absorbtion = glm::vec3(0.0f, 0.0f, 0.0f);	  // Default absorbtion
		}
		doubleSided = true;
	}

	// vec4 shade(const RayHit &hit, const Ray &, const Scene &scene, uint32_t &seed) const override;
};

class ConstantMaterial : public Material {
   public:
	glm::vec3 albedo;

	ConstantMaterial(const glm::vec3 &albedo) : albedo(albedo) {}

	ConstantMaterial(const rapidjson::Value &obj) : Material(obj, true, false) {
		const auto &albedoJSON = obj["albedo"].GetArray();
		albedo = glm::vec3{albedoJSON[0].GetFloat(), albedoJSON[1].GetFloat(), albedoJSON[2].GetFloat()};
	}

	// vec4 shade(const RayHit &, const Ray &, const Scene &, uint32_t &) const override { return
	// glm::vec4(albedo, 1.0f); }
};

}	  // namespace beamcast
