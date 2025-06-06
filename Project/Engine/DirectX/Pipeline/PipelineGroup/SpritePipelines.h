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
public:

	SpritePipelines() = default;
	~SpritePipelines() override;

	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	void AddPipeline(const std::string& fileName, json jsonData);

private:

};

