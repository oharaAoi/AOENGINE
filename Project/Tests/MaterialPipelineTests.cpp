#include "Engine/Module/Components/Materials/MaterialPipelinePolicy.h"
#include <nlohmann/json.hpp>
#include <cassert>
#include <fstream>
#include <iostream>

bool Compatible(bool pbr, const nlohmann::json& pipeline) {
	return AOENGINE::IsMaterialPipelineCompatible(pbr, pipeline.value("vs", ""), pipeline.value("ps", ""),
		pipeline.value("rtvFormat", std::vector<std::string>{}), pipeline.value("primitiveTopologyType", ""));
}

nlohmann::json LoadPipeline(const char* name) {
	std::ifstream file(std::string("Assets/Engine/Pipeline/Object/") + name);
	assert(file.is_open());
	nlohmann::json data;
	file >> data;
	return data.at("Pipeline");
}

int main() {
	for (const char* name : {"Object_Normal.json", "Object_Add.json", "Object_NormalCut.json", "Object_NormalEnviroment.json"}) {
		const auto pipeline = LoadPipeline(name);
		assert(Compatible(false, pipeline));
		assert(!Compatible(true, pipeline));
	}
	const auto pbr = LoadPipeline("Object_PBR.json");
	assert(Compatible(true, pbr));
	assert(!Compatible(false, pbr));
	for (const char* name : {"Object_Normal_Instancing.json", "Object_ShadowMap.json", "Object_GpuParticle.json",
		"Object_ParticleAdd.json", "Object_Skybox.json", "Object_TextureBlendNormal.json", "PostProcess_Normal.json"}) {
		const auto pipeline = LoadPipeline(name);
		assert(!Compatible(false, pipeline));
		assert(!Compatible(true, pipeline));
	}
	auto variant = LoadPipeline("Object_Normal.json");
	variant["blendMode"] = "Add";
	variant["cullingType"] = "NONE";
	variant["depth"] = false;
	assert(Compatible(false, variant));
	variant["rtvFormat"] = {"8_RGBA"};
	assert(!Compatible(false, variant));
	variant = LoadPipeline("Object_Normal.json");
	variant["primitiveTopologyType"] = "Line";
	assert(!Compatible(false, variant));
	std::cout << "Material pipeline compatibility and existing pipeline assets passed.\n";
}
