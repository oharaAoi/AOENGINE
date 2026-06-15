#pragma once
#include "Engine/DirectX/Pipeline/IPipelineGroup.h"

enum class ProcessedScenePSO {
	Normal,
	Grayscale,
	RadialBlur,
	GlitchNoise,
	Vignette,
};

namespace AOENGINE {

/// <summary>
/// Scene全体の描画が終わった後に行うような処理のPipelineをまとめたクラス
/// </summary>
class ProcessedScenePipelines :
	public IPipelineGroup {
public: // コンストラクタ

	ProcessedScenePipelines() = default;
	~ProcessedScenePipelines() override;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device">: デバイス</param>
	/// <param name="dxCompiler">: コンパイラ</param>
	void Init(ID3D12Device* _device, DirectXCompiler* _dxCompiler) override;

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="typeName">: パイプラインのファイル名</param>
	void SetPipeline(ID3D12GraphicsCommandList* _commandList, const std::string& _typeName);

	/// <summary>
	/// パイプラインの追加
	/// </summary>
	/// <param name="fileName">: ファイル名</param>
	/// <param name="jsonData">: jsonデータ</param>
	void AddPipeline(const std::string& _fileName, json _jsonData);

};

}