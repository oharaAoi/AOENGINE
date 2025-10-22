#pragma once
// PSO
#include "Engine/DirectX/Pipeline/PipelineGroup/IPipelineGroup.h"


enum class CsPipelineType {
	HorizontalBlur_Pipeline,
	VerticalBlur_Pipeline,
	GrayScale_Pipeline,
	DepthOfField_Pipeline,
	Skinning_Pipeline,
	Blend_Pipeline,
	Result_Pipeline,

	GpuParticleInit,
	GpuParticleUpdate,
	GpuParticleEnd,
	EmitGpuParticle,
};

/// <summary>
/// ComputeShaderPipelineのクラス
/// </summary>
class ComputeShaderPipeline :
	public IPipelineGroup {
public: // コンストラクタ

	ComputeShaderPipeline() = default;
	~ComputeShaderPipeline() override;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">: デバイス</param>
	/// <param name="dxCompiler">: コンパイラ</param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="typeName">: パイプライン情報ファイルの名前</param>
	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	/// <summary>
	/// パイプラインの追加
	/// </summary>
	/// <param name="fileName">; ファイルの名前</param>
	/// <param name="jsonData">: jsonデータ</param>
	void AddPipeline(const std::string& fileName, json jsonData);

private:

};

