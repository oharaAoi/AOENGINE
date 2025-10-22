#pragma once
#include "Engine/DirectX/Pipeline/PipelineGroup/IPipelineGroup.h"


enum class SpritePSO {
	None,		// ブレンドなし
	Normal,		// 通常αブレンド
	Add,		// 加算
	Subtract,	// 減算
	Multily,	// 乗算
	Screen,		// 反転
};

/// <summary>
/// 2d描画を行うPipelineをまとめたクラス
/// </summary>
class SpritePipelines :
	public IPipelineGroup {
public: // コンストラクタ

	SpritePipelines() = default;
	~SpritePipelines() override;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device">: デバイス</param>
	/// <param name="dxCompiler">: コンパイラ</param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="typeName">: ファイル名</param>
	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	/// <summary>
	/// パイプラインの追加
	/// </summary>
	/// <param name="fileName">: ファイル名</param>
	/// <param name="jsonData">: jsonデータ</param>
	void AddPipeline(const std::string& fileName, json jsonData);

private:

};

