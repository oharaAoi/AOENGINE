#include "AssetsManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Manager/AudioManager.h"

namespace fs = std::filesystem;

AssetsManager* AssetsManager::GetInstance() {
	static AssetsManager instance;
	return &instance;
}

void AssetsManager::Init() {
	// engine用
	LoadTextures(kEngineAssets.textures);
	LoadModels(kEngineAssets.models);
	LoadSounds(kEngineAssets.sounds);

	// game用
	LoadTextures(kGameAssets.textures);
	LoadModels(kGameAssets.models);
	LoadSounds(kGameAssets.sounds);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　textureを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadTextures(const std::string& rootPath) {

	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".png") {
			std::string directory = entry.path().parent_path().string();
			std::string fileName = entry.path().filename().string();
			directory += "/";
			TextureManager::LoadTextureFile(directory, fileName);
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　3dモデルを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadModels(const std::string& rootPath) {
	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {

		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			
			if (ext == ".obj" || ext == ".gltf") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();
				directory += "/";
				ModelManager::LoadModel(directory, fileName);
			}
		}

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　soundを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadSounds(const std::string& rootPath) {
	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {

		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			
			if (ext == ".wav" || ext == ".mp3") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();
				directory += "/";
				AudioManager::LoadAudio(directory, fileName);
			}
		}

	}
}
