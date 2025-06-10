#include "AssetsManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Manager/AudioManager.h"
#include "Engine/Utilities/Logger.h"

namespace fs = std::filesystem;

AssetsManager* AssetsManager::GetInstance() {
	static AssetsManager instance;
	return &instance;
}

void AssetsManager::Init() {
	LoadTextures(kEngineAssets.textures);
	LoadTextures(kGameAssets.textures);

	Logger::CommentLog("Loading Models");
	LoadModels(kEngineAssets.models);
	LoadModels(kGameAssets.models);

	Logger::CommentLog("Loading Sounds");
	LoadSounds(kEngineAssets.sounds);
	LoadSounds(kGameAssets.sounds);

	TextureManager::GetInstance()->LoadStack();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　textureを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadTextures(const std::string& rootPath) {
	TextureManager* manager = TextureManager::GetInstance();
	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
		std::string ext = entry.path().extension().string();
		if (ext == ".png" || ext == ".dds") {
			std::string directory = entry.path().parent_path().string();
			std::string fileName = entry.path().filename().string();
			directory += "/";
			manager->StackTexture(directory, fileName);
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
