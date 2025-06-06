#pragma once
#include <string>
#include <iostream>
#include <filesystem>

/// <summary>
/// Asset関連を管理してくるクラス
/// </summary>
class AssetsManager final {
public:

	struct AssetPaths {
		std::string textures;
		std::string models;
		std::string sounds;
	};

public:

	AssetsManager() = default;
	~AssetsManager() = default;

	AssetsManager(const AssetsManager&) = delete;
	const AssetsManager& operator=(const AssetsManager&) = delete;

	static AssetsManager* GetInstance();

	void Init();

private:

	void LoadTextures(const std::string& rootPath);

	void LoadModels(const std::string& rootPath);

	void LoadSounds(const std::string& rootPath);

private:

	const AssetPaths kEngineAssets = {
		"./Engine/Assets/Load/Textures/",
		"./Engine/Assets/Load/Models/",
		"./Engine/Assets/Load/Sounds/"
	};

	const AssetPaths kGameAssets = {
		"./Game/Assets/Load/Textures/",
		"./Game/Assets/Load/Models/",
		"./Game/Assets/Load/Sounds/"
	};
};