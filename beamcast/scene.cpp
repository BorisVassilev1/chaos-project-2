#include "scene.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

Scene::Scene(NRI &nri, NRICommandQueue &q, const std::string_view &filename) {
	rapidjson::Document doc;
	std::ifstream		 file(filename.data());
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open scene file: " + std::string(filename));
	}
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	rapidjson::ParseResult ok = doc.Parse(str.c_str());
	if(!ok) {
		auto ind = ok.Offset();
		std::cout << str.substr(ind > 20 ? ind - 20 : 0, 40) << std::endl;

		throw std::runtime_error("Failed to parse scene file: " + std::string(filename) + 
								 " Error: " + std::string(GetParseError_En(ok.Code())) + 
								 " Offset: " + std::to_string(ok.Offset()));
	}


	rapidjson::Value meshesJSON;
	if(doc.HasMember("meshes") && doc["meshes"].IsArray()) {
		meshesJSON = doc["meshes"];
	} else if (doc.HasMember("objects") && doc["objects"].IsArray()) {
		meshesJSON = doc["objects"];
	} else {
		throw std::runtime_error("Scene file does not contain 'meshes' or 'objects' array: " + std::string(filename));
	}

	assert(meshesJSON.IsArray());
	for (rapidjson::SizeType i = 0; i < meshesJSON.Size(); ++i) {
		meshes.emplace_back(nri, q, meshesJSON[i]);
	}
}
