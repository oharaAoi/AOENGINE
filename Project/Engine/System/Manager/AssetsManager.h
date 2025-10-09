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
		"./Project/Packages/Engine/Assets/Load/Textures/",
		"./Project/Packages/Engine/Assets/Load/Models/",
		"./Project/Packages/Engine/Assets/Load/Sounds/"
	};

	const AssetPaths kGameAssets = {
		"./Project/Packages/Game/Assets/Load/Textures/",
		"./Project/Packages/Game/Assets/Load/Models/",
		"./Project/Packages/Game/Assets/Load/Sounds/"
	};
};