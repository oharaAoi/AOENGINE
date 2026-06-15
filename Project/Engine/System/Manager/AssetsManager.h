#pragma once
#include <atomic>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Engine/Lib/Path.h"

namespace AOENGINE {

/// <summary>
/// Asset関連を管理してくるクラス
/// </summary>
class AssetsManager final {
public:

	/// <summary>
	/// アセットパス情報
	/// </summary>
	struct AssetPaths {
		std::string textures;
		std::string models;
		std::string sounds;
	};

	struct PipelineAssetPaths {
		std::string object;
		std::string sprite;
		std::string postProcess;
		std::string primitive;
		std::string compute;
	};

	enum class WatchTarget {
		Texture,
		Model,
		Sound,
		GraphicsPipeline,
		ComputePipeline,
		JsonItem,
	};

	struct WatchRoot {
		std::string path;
		WatchTarget target;
		int pipelineType = -1;
	};

	struct FileState {
		std::filesystem::file_time_type writeTime;
		uintmax_t fileSize = 0;
	};

	struct WatchEvent {
		std::filesystem::path filePath;
		WatchTarget target;
		int pipelineType = -1;
		int retryCount = 0;
	};

	struct PendingWatchEvent {
		WatchEvent event;
		FileState state;
		std::chrono::steady_clock::time_point detectedAt;
	};

public:

	AssetsManager() = default;
	~AssetsManager();

	AssetsManager(const AssetsManager&) = delete;
	const AssetsManager& operator=(const AssetsManager&) = delete;

	static AssetsManager* GetInstance();

	void Init();
	void Update();
	void Finalize();

private:

	// Textureの読み込み
	void LoadTextures(const std::string& rootPath);

	// modelの読み込み
	void LoadModels(const std::string& rootPath);

	// soundの読み込み
	void LoadSounds(const std::string& rootPath);

	void BuildWatchRoots();
	void SnapshotWatchedFiles();
	void StartWatchThread();
	void StopWatchThread();
	void WatchLoop();
	void ScanWatchedFiles();
	void EnqueueAssetEvent(const WatchEvent& event);
	void ProcessAssetEvent(const WatchEvent& event);
	bool LoadTextureAsset(const std::filesystem::path& filePath, bool forceReload);
	void LoadModelAsset(const std::filesystem::path& filePath, bool forceReload);
	void LoadSoundAsset(const std::filesystem::path& filePath, bool forceReload);
	void LoadGraphicsPipelineAsset(const std::filesystem::path& filePath, int pipelineType);
	void LoadComputePipelineAsset(const std::filesystem::path& filePath);
	void LoadJsonItemAsset(const std::filesystem::path& filePath);
	bool IsWatchTargetFile(const std::filesystem::path& filePath, WatchTarget target) const;

private:

	const AssetPaths kEngineAssets = {
		kAssetPath + "/Engine/Load/Textures/",
		kAssetPath + "/Engine/Load/Models/",
		kAssetPath + "/Engine/Load/Sounds/"
	};

	const AssetPaths kGameAssets = {
		kAssetPath + "/Game/Load/Textures/",
		kAssetPath + "/Game/Load/Models/",
		kAssetPath + "/Game/Load/Sounds/"
	};

	const PipelineAssetPaths kEnginePipelineAssets = {
		kAssetPath + "/Engine/Pipeline/Object/",
		kAssetPath + "/Engine/Pipeline/Sprite/",
		kAssetPath + "/Engine/Pipeline/PostProcess/",
		kAssetPath + "/Engine/Pipeline/Primitive/",
		kAssetPath + "/Engine/Pipeline/CS/"
	};

	const PipelineAssetPaths kGamePipelineAssets = {
		kAssetPath + "/Game/Pipeline/Object/",
		kAssetPath + "/Game/Pipeline/Sprite/",
		kAssetPath + "/Game/Pipeline/PostProcess/",
		kAssetPath + "/Game/Pipeline/CS/"
	};

	const std::string kJsonItemsObjectPath = kAssetPath + "/Game/GameData/JsonItems/Object/";
	const std::string kJsonItemsEffectPath = kAssetPath + "/Game/GameData/JsonItems/Effect/";

	std::vector<WatchRoot> watchRoots_;
	std::unordered_map<std::string, FileState> knownFiles_;
	std::unordered_map<std::string, PendingWatchEvent> pendingWatchEvents_;
	std::queue<WatchEvent> assetEventQueue_;
	std::unordered_set<std::string> queuedPaths_;
	std::mutex assetEventMutex_;
	std::thread watchThread_;
	std::atomic_bool isWatching_ = false;
};

}
