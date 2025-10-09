#include "ComputeShaderPipelines.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void ComputeShaderPipelines::Finalize() {
	computeShaderPipeline_.reset();
}

void ComputeShaderPipelines::Init(ID3D12Device* device, DirectXCompiler* dxCompiler) {
	assert(device);
	assert(dxCompiler);

	device_ = device;
	dxCompiler_ = dxCompiler;

	computeShaderPipeline_ = std::make_unique<ComputeShaderPipeline>();

	computeShaderPipeline_->Init(device, dxCompiler);
	
	// engine用
	Load(kEngineAssets);
	
	// game用
	Load(kGameAssets);
}

void ComputeShaderPipelines::Load(const std::string& path) {
	if (!fs::exists(path)) {
		std::cerr << "Warning: path not found -> " << path << std::endl;
		return;
	}
	for (const auto& entry : fs::recursive_directory_iterator(path)) {

		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();

			if (ext == ".json") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();

				computeShaderPipeline_->AddPipeline(fileName, LoadJson(directory, fileName));
			}
		}

	}
}

json ComputeShaderPipelines::LoadJson(const std::string& directory, const std::string& fileName) {
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

void ComputeShaderPipelines::SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName) {
	computeShaderPipeline_->SetPipeline(commandList, typeName);
	lastUsedPipeline_ = computeShaderPipeline_->GetLastUsedPipeline();
}