#include "scene.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "glm/ext/matrix_transform.hpp"
#include "utils.hpp"

MeshObject::MeshObject(uint32_t meshIndex, const rapidjson::Value &obj) : meshIndex(meshIndex) {
	transform = glm::identity<glm::mat4>();

	// if (obj.FindMember("material_index") != obj.MemberEnd()) {
	//	materialIndex = obj["material_index"].GetUint() + 1;
	// } else {
	//	materialIndex = 0;	   // Default to first material if not specified TODO: bad
	// }

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

	// const auto& mesh		  = this->scene->meshes[meshIndex];
	// vec3		boundsBase[2] = {mesh.box.min, mesh.box.max};
	// for (int i = 0; i < 8; ++i) {
	//	auto boundPoint	 = vec3(boundsBase[(i & 1)].x, boundsBase[(i & 2)].y, boundsBase[(i & 4)].z);
	//	auto transformed = transform * vec4(boundPoint, 1.0f);
	//	box.add(transformed.xyz());
	// }
}

Scene::Scene(NRI &nri, NRICommandQueue &q, const std::string_view &filename) {
	rapidjson::Document doc;
	std::ifstream		file(filename.data());
	if (!file.is_open()) { throw std::runtime_error("Failed to open scene file: " + std::string(filename)); }
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

		throw std::runtime_error("Failed to parse scene file: " + std::string(filename) + " Error: " +
								 std::string(GetParseError_En(ok.Code())) + " Offset: " + std::to_string(ok.Offset()));
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
}
