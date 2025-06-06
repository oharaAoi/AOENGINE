#include "GraphicsPipelines.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

GraphicsPipelines::GraphicsPipelines() {
}

GraphicsPipelines::~GraphicsPipelines() {
}

void GraphicsPipelines::Finalize() {
	obj3dPipeline_.reset();
	spritePipeline_.reset();
	processedScenePipeline_.reset();
}

void GraphicsPipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler) {
	assert(device);
	assert(dxCompiler);
	
	device_ = device;
	dxCompiler_ = dxCompiler;
	
	obj3dPipeline_ = std::make_unique<Object3dPipelines>();
	spritePipeline_ = std::make_unique<SpritePipelines>();
	processedScenePipeline_ = std::make_unique<ProcessedScenePipelines>();

	obj3dPipeline_->Init(device, dxCompiler);
	spritePipeline_->Init(device, dxCompiler);
	processedScenePipeline_->Init(device, dxCompiler);

	// engine用
	Load(kEngineAssets.object, PSOType::Object3d);
	Load(kEngineAssets.sprite, PSOType::Sprite);
	Load(kEngineAssets.postProcess, PSOType::ProcessedScene);
	// game用
	Load(kGameAssets.object, PSOType::Object3d);
	Load(kGameAssets.sprite, PSOType::Sprite);
	Load(kGameAssets.postProcess, PSOType::ProcessedScene);
}

void GraphicsPipelines::Load(const std::string& path, PSOType type) {
	for (const auto& entry : fs::recursive_directory_iterator(path)) {

		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();

			if (ext == ".json") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();
				
				switch (type) {
				case PSOType::Object3d:
					obj3dPipeline_->AddPipeline(fileName, LoadJson(directory, fileName));
					break;
				case PSOType::Sprite:
					spritePipeline_->AddPipeline(fileName, LoadJson(directory, fileName));
					break;
				case PSOType::ProcessedScene:
					processedScenePipeline_->AddPipeline(fileName, LoadJson(directory, fileName));
					break;
				case PSOType::Primitive:
					break;
				default:
					break;
				}
			}
		}

	}
}

json GraphicsPipelines::LoadJson(const std::string& directory, const std::string& fileName) {
	std::string filePath = directory + "/" + fileName;

	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath + ".json";
		assert(0);
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// 値の追加
	return root;
}

void GraphicsPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, PSOType type, const std::string& typeName) {
	switch (type) {
	case PSOType::Object3d:
		obj3dPipeline_->SetPipeline(commandList, typeName);
		lastUsedPipeline_ = obj3dPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Sprite:
		spritePipeline_->SetPipeline(commandList, typeName);
		lastUsedPipeline_ = spritePipeline_->GetLastUsedPipeline();
		break;
	case PSOType::ProcessedScene:
		processedScenePipeline_->SetPipeline(commandList, typeName);
		lastUsedPipeline_ = processedScenePipeline_->GetLastUsedPipeline();
		break;
	default:
		break;
	}
}