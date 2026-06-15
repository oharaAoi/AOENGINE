#pragma once
#include "Engine/DirectX/Pipeline/IPipelineGroup.h"

namespace AOENGINE {

/// <summary>
/// 線描がのパイプライン
/// </summary>
class PrimitivePipeline :
	public IPipelineGroup {
public: // コンストラクタ

	PrimitivePipeline() = default;
	~PrimitivePipeline() override;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dxCompiler"></param>
	void Init(ID3D12Device* _device, AOENGINE::DirectXCompiler* _dxCompiler) override;

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="typeName">: パイプラインのタイプ</param>
	void SetPipeline(ID3D12GraphicsCommandList* _commandList, const std::string& _typeName);

	/// <summary>
	/// パイプラインの追加
	/// </summary>
	/// <param name="fileName">: ファイル名</param>
	/// <param name="jsonData">: jsonデータ</param>
	void AddPipeline(const std::string& _fileName, json _jsonData);

};

}