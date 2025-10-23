#pragma once
#include <string>
#include "Engine/DirectX/Pipeline/PipelineGroup/Object3dPipelines.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/SpritePipelines.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/ProcessedScenePipelines.h"

enum class PSOType{
	Object3d,
	Sprite,
	ProcessedScene,
	Primitive,
};

/// <summary>
/// 描画のパイプラインをまとめたクラス
/// </summary>
class GraphicsPipelines {
public:

	/// <summary>
	/// ファイルパスの種類
	/// </summary>
	struct AssetPaths {
		std::string object;
		std::string sprite;
		std::string postProcess;
	};

public: // コンストラクタ

	GraphicsPipelines();
	~GraphicsPipelines();

public :

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="dxCompiler">コンパイラー</param>
	/// <param name="shaders">シェーダーファイルパスをまとめたクラス</param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler);

	/// <summary>
	/// 読み込み関数
	/// </summary>
	/// <param name="path">: ファイルパス</param>
	/// <param name="type">; パイプラインのタイプ</param>
	void Load(const std::string& path, PSOType type);

	/// <summary>
	/// jsonデータ読み込み
	/// </summary>
	/// <param name="directory">: ディレクトリパス</param>
	/// <param name="fileName">: ファイル名</param>
	/// <returns></returns>
	json LoadJson(const std::string& directory, const std::string& fileName);

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="type">: タイプ</param>
	/// <param name="typeName">: パイプラインのファイル名</param>
	void SetPipeline(ID3D12GraphicsCommandList* commandList, PSOType type, const std::string& typeName);

	/// <summary>
	/// 最後に使用したパイプラインのポインタを返す
	/// </summary>
	/// <returns></returns>
	Pipeline* GetLastUsedPipeline() const { return lastUsedPipeline_; }

private:

	const AssetPaths kEngineAssets = {
		"./Project/Packages/Engine/Assets/Pipeline/Object/",
		"./Project/Packages/Engine/Assets/Pipeline/Sprite/",
		"./Project/Packages/Engine/Assets/Pipeline/PostProcess/"
	};

	const AssetPaths kGameAssets = {
		"./Project/Packages/Game/Assets/Pipeline/Object/",
		"./Project/Packages/Game/Assets/Pipeline/Sprite/",
		"./Project/Packages/Game/Assets/Pipeline/PostProcess/"
	};

	std::unique_ptr<Object3dPipelines> obj3dPipeline_;
	std::unique_ptr<SpritePipelines> spritePipeline_;
	std::unique_ptr<ProcessedScenePipelines> processedScenePipeline_;

	// inputLayout
	InputLayout inputLayout_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;

	Pipeline* lastUsedPipeline_;

};

